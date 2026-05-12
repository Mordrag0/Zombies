// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZFireModeAutomatic.h"

void UZFireModeAutomatic::FireComplete()
{
	Super::FireComplete();

	if (bHeldDown && CanFire())
	{
		Fire();
	}
}

