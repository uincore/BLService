#pragma once

#include "stdafx.h"
#include "sysdep.h"

class CConfig
{
public:
	CConfig(void);
	~CConfig(void);
	void ReadINI();
};

