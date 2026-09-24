#include "pch.h"

#include "control/MouseButton.h"

TEST(MouseButtonTest, FirstReleaseHasNoPreviousRelease) {
	MouseButton button;

	EXPECT_FALSE(button.hasPreviousRelease());
}

TEST(MouseButtonTest, PreviousReleaseIsRecognizedAfterTimestampIsSet) {
	MouseButton button;
	button.prevUp = 1.f;

	EXPECT_TRUE(button.hasPreviousRelease());
}
