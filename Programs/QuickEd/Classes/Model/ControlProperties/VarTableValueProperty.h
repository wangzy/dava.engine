#pragma once

#include "IntrospectionProperty.h"
#include <UI/Properties/VarTable.h>

class VarTableValueProperty : public IntrospectionProperty
{
public:
    VarTableValueProperty(DAVA::BaseObject* object, const DAVA::String& name, const DAVA::Reflection& ref, const IntrospectionProperty* prototypeProperty);
    ~VarTableValueProperty() override;

    DAVA::Any GetDefaultLocalValue(const DAVA::String& propertyName);
    DAVA::Any GetLocalValue(const DAVA::String& name) const;
    void ResetLocalValue(const DAVA::String& propertyName);
    void SetLocalValue(const DAVA::String& name, const DAVA::Any& value);
    bool IsOverriddenLocally(const DAVA::String& name);
    void UpdateRealProperties();

    void SetValue(const DAVA::Any& newValue) override;
    void SetDefaultValue(const DAVA::Any& newValue) override;
    void ResetValue() override;

    bool HasAnySubPropertyOverridden() const;
    bool IsSubPropertyOverridden(const DAVA::FastName& propertyName) const;
    void SetPropertyOverridden(const DAVA::FastName& name, bool value);

    void SetSubOverriddenIfNotEqual(DAVA::VarTable& varTable, DAVA::VarTable& varTableDefault);
    void SetSubOverriddenIfNotEqualDefaultValues(DAVA::VarTable& varTable);

    void OverrideSubProperties(const DAVA::VarTable& varTable);
    DAVA::String GetNamesString(const DAVA::String& sep = ",") const;

    DAVA::Set<DAVA::FastName> propertyOverridden;
};
