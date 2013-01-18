#include "DAVAEngine.h"
#include "QtPropertyEditor/QtProperyData/QtPropertyDataDavaVariant.h"

QtPropertyDataDavaVariant::QtPropertyDataDavaVariant(const DAVA::VariantType &value)
	: curVariantValue(value)
{
	// set special flags
	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_BOOLEAN:
		SetFlags(FLAG_IS_CHECKABLE | FLAG_IS_NOT_EDITABLE);
		break;
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
	case DAVA::VariantType::TYPE_BYTE_ARRAY:
	case DAVA::VariantType::TYPE_POINTER:
		SetFlags(FLAG_IS_DISABLED);
		break;
            
    case DAVA::VariantType::TYPE_MATRIX2:
    case DAVA::VariantType::TYPE_MATRIX3:
    case DAVA::VariantType::TYPE_MATRIX4:
        SetFlags(FLAG_IS_NOT_EDITABLE);
        break;
            
	case DAVA::VariantType::TYPE_FLOAT:
	case DAVA::VariantType::TYPE_INT32:
	case DAVA::VariantType::TYPE_INT64:
	case DAVA::VariantType::TYPE_UINT32:
	case DAVA::VariantType::TYPE_UINT64:
	case DAVA::VariantType::TYPE_STRING:
	case DAVA::VariantType::TYPE_VECTOR2:
	case DAVA::VariantType::TYPE_VECTOR3:
	case DAVA::VariantType::TYPE_VECTOR4:
	default:
		break;
	}

	ChildsCreate();
}

QtPropertyDataDavaVariant::~QtPropertyDataDavaVariant()
{ }

const DAVA::VariantType& QtPropertyDataDavaVariant::GetVariantValue() const
{
	return curVariantValue;
}

void QtPropertyDataDavaVariant::SetVariantValue(const DAVA::VariantType& value)
{
	DVASSERT(curVariantValue.type == value.type);
	curVariantValue = value;
}

QVariant QtPropertyDataDavaVariant::GetValueInternal()
{
	QVariant v;

	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_BOOLEAN:
		v = curVariantValue.AsBool();
		break;
	case DAVA::VariantType::TYPE_FLOAT:
		v = curVariantValue.AsFloat();
		break;
	case DAVA::VariantType::TYPE_INT32:
		v = curVariantValue.AsInt32();
		break;
	case DAVA::VariantType::TYPE_INT64:
		v = curVariantValue.AsInt64();
		break;
	case DAVA::VariantType::TYPE_UINT32:
		v = curVariantValue.AsUInt32();
		break;
	case DAVA::VariantType::TYPE_UINT64:
		v = curVariantValue.AsUInt64();
		break;
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
		v = FromKeyedArchive(curVariantValue.AsKeyedArchive());
		break;
	case DAVA::VariantType::TYPE_STRING:
		v = curVariantValue.AsString().c_str();
		break;
	case DAVA::VariantType::TYPE_MATRIX2:
		v = FromMatrix2(curVariantValue.AsMatrix2());
		break;
	case DAVA::VariantType::TYPE_MATRIX3:
		v = FromMatrix3(curVariantValue.AsMatrix3());
		break;
	case DAVA::VariantType::TYPE_MATRIX4:
		v = FromMatrix4(curVariantValue.AsMatrix4());
		break;
	case DAVA::VariantType::TYPE_VECTOR2:
		v = FromVector2(curVariantValue.AsVector2());
		break;
	case DAVA::VariantType::TYPE_VECTOR3:
		v = FromVector3(curVariantValue.AsVector3());
		break;
	case DAVA::VariantType::TYPE_VECTOR4:
		v = FromVector4(curVariantValue.AsVector4());
		break;

	case DAVA::VariantType::TYPE_BYTE_ARRAY:
	default:
		break;
	}

	return v;
}

void QtPropertyDataDavaVariant::SetValueInternal(const QVariant &value)
{
	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_BOOLEAN:
		curVariantValue.SetBool(value.toBool());
		break;
	case DAVA::VariantType::TYPE_FLOAT:
		curVariantValue.SetFloat(value.toFloat());
		break;
	case DAVA::VariantType::TYPE_INT32:
		curVariantValue.SetInt32(value.toInt());
		break;
	case DAVA::VariantType::TYPE_INT64:
		curVariantValue.SetInt64(value.toLongLong());
		break;
	case DAVA::VariantType::TYPE_UINT32:
		curVariantValue.SetUInt32(value.toUInt());
		break;
	case DAVA::VariantType::TYPE_UINT64:
		curVariantValue.SetUInt64(value.toULongLong());
		break;
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
		ToKeyedArchive(value);
		break;
	case DAVA::VariantType::TYPE_STRING:
		curVariantValue.SetString(value.toString().toStdString());
		break;
	case DAVA::VariantType::TYPE_MATRIX2:
		ToMatrix2(value);
		break;
	case DAVA::VariantType::TYPE_MATRIX3:
		ToMatrix3(value);
		break;
	case DAVA::VariantType::TYPE_MATRIX4:
		ToMatrix4(value);
		break;
	case DAVA::VariantType::TYPE_VECTOR2:
		ToVector2(value);
		break;
	case DAVA::VariantType::TYPE_VECTOR3:
		ToVector3(value);
		break;
	case DAVA::VariantType::TYPE_VECTOR4:
		ToVector4(value);
		break;

	case DAVA::VariantType::TYPE_BYTE_ARRAY:
	default:
		break;
	}
}

void QtPropertyDataDavaVariant::ChildChanged(const QString &key, QtPropertyData *data)
{
	MeSetFromChilds(key, data);
}

void QtPropertyDataDavaVariant::ChildNeedUpdate()
{
	ChildsSetFromMe();
}

void QtPropertyDataDavaVariant::ChildsCreate()
{
	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
		{
			DAVA::KeyedArchive *archive = curVariantValue.AsKeyedArchive();
			DAVA::Map<DAVA::String, DAVA::VariantType*> data = archive->GetArchieveData();
			DAVA::Map<DAVA::String, DAVA::VariantType*>::iterator i = data.begin();

			for(; i != data.end(); ++i)
			{
				ChildAdd(i->first.c_str(), new QtPropertyDataDavaVariant(*(i->second)));
			}
		}
		break;
	case DAVA::VariantType::TYPE_MATRIX2:
		break;
	case DAVA::VariantType::TYPE_MATRIX3:
		break;
	case DAVA::VariantType::TYPE_MATRIX4:
		break;
	case DAVA::VariantType::TYPE_VECTOR2:
		{
			DAVA::Vector2 vec = curVariantValue.AsVector2();
			ChildAdd("X", vec.x);
			ChildAdd("Y", vec.y);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR3:
		{
			DAVA::Vector3 vec = curVariantValue.AsVector3();
			ChildAdd("X", vec.x);
			ChildAdd("Y", vec.y);
			ChildAdd("Z", vec.z);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR4:
		{
			DAVA::Vector4 vec = curVariantValue.AsVector4();
			ChildAdd("X", vec.x);
			ChildAdd("Y", vec.y);
			ChildAdd("Z", vec.z);
			ChildAdd("W", vec.w);
		}
		break;
	}
}

void QtPropertyDataDavaVariant::ChildsSetFromMe()
{
	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
		{
			// No way to change whole archive
			// so don't need to re-set childs
		}
		break;
	case DAVA::VariantType::TYPE_MATRIX2:
		break;
	case DAVA::VariantType::TYPE_MATRIX3:
		break;
	case DAVA::VariantType::TYPE_MATRIX4:
		break;
	case DAVA::VariantType::TYPE_VECTOR2:
		{
			DAVA::Vector2 vec = curVariantValue.AsVector2();
			ChildGet("X")->SetValue(vec.x);
			ChildGet("Y")->SetValue(vec.y);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR3:
		{
			DAVA::Vector3 vec = curVariantValue.AsVector3();
			ChildGet("X")->SetValue(vec.x);
			ChildGet("Y")->SetValue(vec.y);
			ChildGet("Z")->SetValue(vec.z);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR4:
		{
			DAVA::Vector4 vec = curVariantValue.AsVector4();
			ChildGet("X")->SetValue(vec.x);
			ChildGet("Y")->SetValue(vec.y);
			ChildGet("Z")->SetValue(vec.z);
			ChildGet("W")->SetValue(vec.w);
		}
		break;
	}
}

void QtPropertyDataDavaVariant::MeSetFromChilds(const QString &lastChangedChildKey, QtPropertyData *lastChangedChildData)
{
	switch(curVariantValue.type)
	{
	case DAVA::VariantType::TYPE_KEYED_ARCHIVE:
		{
			QtPropertyDataDavaVariant *childVariantData = (QtPropertyDataDavaVariant *) lastChangedChildData;
			DAVA::KeyedArchive *archive = curVariantValue.AsKeyedArchive();

			if(NULL != archive && NULL != childVariantData)
			{
				archive->SetVariant(lastChangedChildKey.toStdString(), &(childVariantData->curVariantValue));
			}
		}
		break;
	case DAVA::VariantType::TYPE_MATRIX2:
		break;
	case DAVA::VariantType::TYPE_MATRIX3:
		break;
	case DAVA::VariantType::TYPE_MATRIX4:
		break;
	case DAVA::VariantType::TYPE_VECTOR2:
		{
			DAVA::Vector2 vec;
			vec.x = ChildGet("X")->GetValue().toFloat();
			vec.y = ChildGet("Y")->GetValue().toFloat();
			curVariantValue.SetVector2(vec);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR3:
		{
			DAVA::Vector3 vec;
			vec.x = ChildGet("X")->GetValue().toFloat();
			vec.y = ChildGet("Y")->GetValue().toFloat();
			vec.z = ChildGet("Z")->GetValue().toFloat();
			curVariantValue.SetVector3(vec);
		}
		break;
	case DAVA::VariantType::TYPE_VECTOR4:
		{
			DAVA::Vector4 vec;
			vec.x = ChildGet("X")->GetValue().toFloat();
			vec.y = ChildGet("Y")->GetValue().toFloat();
			vec.z = ChildGet("Z")->GetValue().toFloat();
			vec.w = ChildGet("W")->GetValue().toFloat();
			curVariantValue.SetVector4(vec);
		}
		break;
	}
}


QVariant QtPropertyDataDavaVariant::FromKeyedArchive(DAVA::KeyedArchive *archive)
{
	QVariant v;

	if(NULL != archive)
	{
		v = QString("KeyedArchive");
	}

	return v;
}

QVariant QtPropertyDataDavaVariant::FromVector4(const DAVA::Vector4 &vector)
{
	QVariant v;

	v = QString().sprintf("[%g, %g, %g, %g]", vector.x, vector.y, vector.z, vector.w);

	return v;
}

QVariant QtPropertyDataDavaVariant::FromVector3(const DAVA::Vector3 &vector)
{
	QVariant v;

	v = QString().sprintf("[%g, %g, %g]", vector.x, vector.y, vector.z);

	return v;
}

QVariant QtPropertyDataDavaVariant::FromVector2(const DAVA::Vector2 &vector)
{
	QVariant v;

	v = QString().sprintf("[%g, %g]", vector.x, vector.y);

	return v;
}

QVariant QtPropertyDataDavaVariant::FromMatrix4(const DAVA::Matrix4 &matrix)
{
	QVariant v;

	v = QString().sprintf("[%g, %g, %g, %g]\n[%g, %g, %g, %g]\n[%g, %g, %g, %g]\n[%g, %g, %g, %g]",
                          matrix._00, matrix._01, matrix._02, matrix._03,
                          matrix._10, matrix._11, matrix._12, matrix._13,
                          matrix._20, matrix._21, matrix._22, matrix._23,
                          matrix._30, matrix._31, matrix._32, matrix._33
                          );

	return v;
}

QVariant QtPropertyDataDavaVariant::FromMatrix3(const DAVA::Matrix3 &matrix)
{
	QVariant v;

	v = QString().sprintf("[%g, %g, %g]\n[%g, %g, %g]\n[%g, %g, %g]",
                          matrix._00, matrix._01, matrix._02,
                          matrix._10, matrix._11, matrix._12,
                          matrix._20, matrix._21, matrix._22
                          );

	return v;
}

QVariant QtPropertyDataDavaVariant::FromMatrix2(const DAVA::Matrix2 &matrix)
{
	QVariant v;

	v = QString().sprintf("([%g, %g]\n[%g, %g])",
		matrix._00, matrix._01,
		matrix._10, matrix._11);

	return v;
}

void QtPropertyDataDavaVariant::ToKeyedArchive(const QVariant &value)
{
	// No way to set whole archive
}

void QtPropertyDataDavaVariant::ToVector4(const QVariant &value)
{
	// TODO:
	// ...
}

void QtPropertyDataDavaVariant::ToVector3(const QVariant &value)
{
	// TODO:
	// ...
}

void QtPropertyDataDavaVariant::ToVector2(const QVariant &value)
{
	// TODO:
	// ...
}

void QtPropertyDataDavaVariant::ToMatrix4(const QVariant &value)
{
	// TODO:
	// ...
}

void QtPropertyDataDavaVariant::ToMatrix3(const QVariant &value)
{
	// TODO:
	// ...
}

void QtPropertyDataDavaVariant::ToMatrix2(const QVariant &value)
{
	// TODO:
	// ...
}
