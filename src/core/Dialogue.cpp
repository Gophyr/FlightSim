#include "Dialogue.h"

DialogueTree::DialogueTree(std::string filename)
{
	IrrXMLReader* xml = createIrrXMLReader(filename.c_str());
	if (!xml) return;

	delete xml;
}