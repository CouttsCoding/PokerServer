#pragma once
#include "iNetMessage.h"

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

// It's good to be organized. When game's protocols get really complicated,
// some programmers divide it by category, some do it by communication direction.
// This is how it's organized below. So all messages that go client -> server are
// in one file, the inverse in another file, and ones that go both ways in a 
// third file.

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

#include "clientMessages.h"
#include "serverMessages.h"
#include "duplexMessages.h"