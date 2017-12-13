#!/usr/bin/env python
import sys
import os
import argparse
import subprocess

class Preparation:

    white_list_dirs = set()
    white_list_files = set()
    clear_list_dirs = set()
    clear_list_files = set()

    EXCLUDE = set(['.git'])

    def __init__(self, base_path, repo):
        self.repo = repo
        self.base_path = os.path.abspath(base_path)
        self.read_white_list()

        self.get_clear_list()
        self.remove_extra()
        self.remove_big_files()

    def add_white_path(self, path):
        if os.path.isfile(path):
            self.white_list_files.add(path)

        if os.path.isdir(path):
            self.white_list_dirs.add(path)


    def add_file_to_clear_path(self, path):
        self.clear_list_files.add(path)

    def add_dir_to_clear_path(self, path):
        self.clear_list_dirs.add(path)

    def read_white_list(self):
        try:
            files_list = open(os.path.abspath(self.base_path + '/Bin/RepoTools/Scripts/GithubTools/white_list.txt'))
            for line in files_list:
                path = os.path.abspath(self.base_path + line.partition('#')[0].replace('\n', ''))
                if len(path) != 0:
                    if "!" in path:
                        path = path.replace("!", "")
                        if os.path.isfile(path):
                            self.add_file_to_clear_path(path)
                        if os.path.isdir(path):
                            self.add_dir_to_clear_path(path);
                    else:
                        if path != self.base_path:
                            self.add_white_path(path)

        except IOError:
            print "##teamcity[message text='Unable to open file white_list.txt' status='ERROR']"
            sys.exit(3)

    def get_clear_list(self):

        for root, dirs, files in os.walk(self.base_path):
            dirs[:] = [d for d in dirs if d not in self.EXCLUDE]

            for file_name in files:
                if not [True for white_path in self.white_list_files if os.path.join(root, file_name) in white_path]:
                    self.add_file_to_clear_path(os.path.join(root, file_name))

            for dir_name in dirs:
                if not [True for white_path in self.white_list_dirs if os.path.join(root, dir_name) in white_path]:
                    self.add_dir_to_clear_path(os.path.join(root, dir_name))

    def remove_extra(self):

        os.chdir(self.base_path)

        sys.stdout.write('Deleting files from repo:\n')
        sys.stdout.flush()

        for path in self.clear_list_files:
            cmd = 'java -jar {}/bfg-1.12.15.jar --no-blob-protection --delete-files {} {}/.git'.format(
                self.base_path + '/Bin/RepoTools/Scripts/GithubTools', os.path.basename(path), self.repo)
            self.execute_bfg_cmd(cmd)

        for path in self.clear_list_dirs:
            cmd = 'java -jar {}/bfg-1.12.15.jar --no-blob-protection --delete-folders {} {}/.git'.format(
                self.base_path + '/Bin/RepoTools/Scripts/GithubTools', path, self.repo)
            self.execute_bfg_cmd(cmd)

    def execute_bfg_cmd(self, cmd):
        sys.stdout.write(cmd + '\n')
        sys.stdout.flush()
        try:
            cmd_log = subprocess.check_output(cmd, shell=True)
            sys.stdout.write(cmd_log)
            sys.stdout.flush()
            if 'Usage: bfg' in cmd_log:
                print "##teamcity[message text='Error removing big files' errorDetails='Not passed important parameters' status='ERROR']"
                sys.exit(3)
        except subprocess.CalledProcessError as cmd_except:
            print "##teamcity[message text='Error removing extra files' errorDetails='%s' status='ERROR']" % cmd_except.output
            sys.exit(3)

    def remove_big_files(self):
        cmd = 'java -jar {}/bfg-1.12.15.jar --no-blob-protection --strip-blobs-bigger-than 99M {}'.format(self.base_path + '/Bin/RepoTools/Scripts/GithubTools', self.repo)
        sys.stdout.write(cmd + '\n')
        sys.stdout.flush()
        try:
            cmd_log = subprocess.check_output(cmd, shell=True)
            sys.stdout.write(cmd_log)
            sys.stdout.flush()
            if 'Usage: bfg' in cmd_log:
                print "##teamcity[message text='Error removing big files' errorDetails='Not passed important parameters' status='ERROR']"
                sys.exit(3)
        except subprocess.CalledProcessError as cmd_except:
            print "##teamcity[message text='Error removing big files' errorDetails='%s' status='ERROR']" % cmd_except.output
            sys.exit(3)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Utility tool to delete files from the repository that are not in the whitelist.')
    parser.add_argument('--path', required=False, help='Example: --path /var/git/repo', default='C:\Users\m_molokovskih\Wargaming\dava.framework')
    parser.add_argument('--repo', required=False, help='Example: --repo my-repo.git', default='C:\Users\m_molokovskih\Wargaming\dava.framework')

    args = parser.parse_args()

    preparation = Preparation(args.path, args.repo)
