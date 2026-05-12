// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZNetworkTypes.h"

class AZCharacter;
enum class EZCharacterActivity : uint32;

/**
 * 
 */
class ZOMBIESEDITOR_API SZCharacterStateWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SZCharacterStateWidget) {}
		SLATE_ARGUMENT(TWeakObjectPtr<AZCharacter>, Character)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SZCharacterStateWidget();

private:
	void RebuildContent();
	
	void OnActivityStarted(const FZInteractionState& InteractionState);
	void OnActivityEnding(EZCharacterActivity Activity);
	void OnActivityStopped(EZCharacterActivity Activity);

	TWeakObjectPtr<AZCharacter> Character;
	TSharedPtr<SVerticalBox> ContentBox;
	TSharedPtr<SListView<TSharedPtr<EZCharacterActivity>>> ActivityList;
	
	TArray<TSharedPtr<EZCharacterActivity>> CurrentActivities;
};

