#pragma once
#include "ControlBase.h"
class TreeView : public ControlBase
{
public:
	TreeView();
	virtual ~TreeView() {
		destroy();
	}
};

