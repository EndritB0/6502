#include "pch.h"

#include <CPU.h>

TEST(HarnessSmokeTest, MemoryIsSixtyFourKilobytes)
{
	EXPECT_EQ(MOS6502::Memory::MemorySize, 1024u * 64u);
}
