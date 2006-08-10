#ifndef _DIAGRAMCONTROLFACTORY_H_
#define _DIAGRAMCONTROLFACTORY_H_

#include "DiagramEditor\DiagramEntity.h"
#include "xml.h"

class CDiagramControlFactory {

public:
	static CDiagramEntity* CreateFromString(XML::Node* node);
};

#endif // _DIAGRAMCONTROLFACTORY_H_