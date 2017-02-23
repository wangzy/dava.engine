#pragma once

#include "TArc/Controls/PropertyPanel/ProxyComponentValue.h"
#include "TArc/Controls/PropertyPanel/PropertyModelExtensions.h"
#include "TArc/Controls/PropertyPanel/DefaultEditorDrawers.h"
#include "TArc/Controls/PropertyPanel/DefaultValueCompositors.h"

#include <Reflection/ReflectedMeta.h>

namespace DAVA
{
namespace TArc
{
class FlagsComponentValue : public ProxyComponentValue<FlagsEditorDrawer, EnumValueCompositor>
{
public:
    FlagsComponentValue() = default;

protected:
    QWidget* AcquireEditorWidget(QWidget* parent, const QStyleOptionViewItem& option) override;

private:
    Any GetValueAny() const;
    void SetValueAny(const Any& newValue);

    bool IsReadOnly() const;

private:
    DAVA_VIRTUAL_REFLECTION(FlagsComponentValue, ProxyComponentValue<FlagsEditorDrawer, EnumValueCompositor>);
};
}
}
