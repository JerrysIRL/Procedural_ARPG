#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"

// Make one macro for all predefined macros that are typically used for Gameplay Attribute Data classes
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define ATTRIBUTE_GETTERS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define PERCENT_DAMAGE_ATTRIBUTE(name) \
	FGameplayAttributeData Percentage##name##Damage; \
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Percentage##name##Damage);

#define FLAT_DAMAGE_ATTRIBUTE(name) \
	FGameplayAttributeData Flat##name##Damage; \
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Flat##name##Damage);
	
#define PERCENT_RESISTANCE_ATTRIBUTE(name) \
	FGameplayAttributeData Percentage##name##Resistance; \
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Percentage##name##Resistance);

#define FLAT_RESISTANCE_ATTRIBUTE(name) \
	FGameplayAttributeData Flat##name##Resistance; \
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Flat##name##Resistance);
	