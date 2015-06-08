/*==================================================================================
 Copyright (c) 2008, binaryzebra
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the binaryzebra nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =====================================================================================*/


#include "UISizeHintComponent.h"

#include "UILayoutSystem.h"

namespace DAVA
{
    UISizeHintComponent::UISizeHintComponent()
        : horizontalPolicy(FIXED_SIZE)
        , horizontalValue(100)
        , verticalPolicy(FIXED_SIZE)
        , verticalValue(25)
    {
        
    }
    
    UISizeHintComponent::UISizeHintComponent(const UISizeHintComponent &src)
        : horizontalPolicy(src.horizontalPolicy)
        , horizontalValue(src.horizontalValue)
        , verticalPolicy(src.verticalPolicy)
        , verticalValue(src.verticalValue)
    {
        
    }
    
    UISizeHintComponent::~UISizeHintComponent()
    {
        
    }
    
    UISizeHintComponent* UISizeHintComponent::Clone()
    {
        return new UISizeHintComponent(*this);
    }
    
    UISizeHintComponent::eSizePolicy UISizeHintComponent::GetHorizontalPolicy() const
    {
        return horizontalPolicy;
    }
    
    void UISizeHintComponent::SetHorizontalPolicy(eSizePolicy policy)
    {
        horizontalPolicy = policy;
    }
    
    float UISizeHintComponent::GetHorizontalValue() const
    {
        return horizontalValue;
    }
    
    void UISizeHintComponent::SetHorizontalValue(float32 value)
    {
        horizontalValue = value;
    }
    
    UISizeHintComponent::eSizePolicy UISizeHintComponent::GetVerticalPolicy() const
    {
        return verticalPolicy;
    }
    
    void UISizeHintComponent::SetVerticalPolicy(eSizePolicy policy)
    {
        verticalPolicy = policy;
    }
    
    float UISizeHintComponent::GetVerticalValue() const
    {
        return verticalValue;
    }
    
    void UISizeHintComponent::SetVerticalValue(float32 value)
    {
        verticalValue = value;
    }
    
    UISizeHintComponent::eSizePolicy UISizeHintComponent::GetPolicyByAxis(int32 axis) const
    {
        switch (axis)
        {
            case UILayoutSystem::AXIS_X:
                return horizontalPolicy;
                
            case UILayoutSystem::AXIS_Y:
                return verticalPolicy;
                
            default:
                DVASSERT(false);
                return FIXED_SIZE;
        }
    }
    
    float UISizeHintComponent::GetValueByAxis(int32 axis) const
    {
        switch (axis)
        {
            case UILayoutSystem::AXIS_X:
                return horizontalValue;
                
            case UILayoutSystem::AXIS_Y:
                return verticalValue;
                
            default:
                DVASSERT(false);
                return 0.0f;
        }
    }

    int32 UISizeHintComponent::GetHorizontalPolicyAsInt() const
    {
        return GetHorizontalPolicy();
    }
    
    void UISizeHintComponent::SetHorizontalPolicyFromInt(int32 policy)
    {
        SetHorizontalPolicy(static_cast<eSizePolicy>(policy));
    }
    
    int32 UISizeHintComponent::GetVerticalPolicyAsInt() const
    {
        return GetVerticalPolicy();
    }
    
    void UISizeHintComponent::SetVerticalPolicyFromInt(int32 policy)
    {
        SetVerticalPolicy(static_cast<eSizePolicy>(policy));
    }

}
