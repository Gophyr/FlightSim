#include "Dialogue.h"
#include "IrrlichtUtils.h"
#include "DialogueEffects.h"
#include <iostream>

const std::unordered_map<std::wstring, DialogueCallback> dialogueEffects {
	{L"small_supply", smallSupply},
	{L"medium_supply", mediumSupply},
	{L"large_supply", largeSupply},
	{L"small_ammo", smallAmmo},
	{L"medium_ammo", mediumAmmo},
	{L"large_ammo", largeAmmo},
	{L"none", noDialogueEffect},
	{L"exit", exitDialogue}
};

DialogueTree::DialogueTree(std::string filename)
{
	IrrXMLReader* xml = createIrrXMLReader(filename.c_str());
	if (!xml) return;

	const stringw treeTag(L"tree");
	const stringw textTag(L"text");
	const stringw nodeTag(L"node");
	const stringw speakerListTag(L"speakers");
	const stringw speakerTag(L"speaker");
	const stringw choiceTag(L"choice");

	stringw current;
	std::wstring currentNodeId;

	while (xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (current.empty() && treeTag.equals_ignore_case(xml->getNodeName())) {
				current = treeTag;
				if(!m_parseTreeInfo(xml)) return;
			}

			if (current == treeTag && speakerListTag.equals_ignore_case(xml->getNodeName())) {
				current = speakerListTag;
			}
			if (current == speakerListTag && speakerTag.equals_ignore_case(xml->getNodeName())) {
				std::string spkr = xml->getAttributeValueSafe("name");
				if (!spkr.empty()) m_speakers.push_back(wstr(spkr));
			}

			if (current == treeTag && nodeTag.equals_ignore_case(xml->getNodeName())) {
				std::string data(xml->getAttributeValueSafe("id"));
				if (!data.empty()) {
					std::wstring id = wstr(data);
					m_allNodes[id] = DialogueNode(id);
					currentNodeId = id;
				}
				current = nodeTag;
			}
			else if (current == nodeTag && !currentNodeId.empty()) {
				if (speakerTag.equals_ignore_case(xml->getNodeName())) {
					std::string name = xml->getAttributeValueSafe("name");
					if(!name.empty()) m_allNodes[currentNodeId].setSpeaker(wstr(name));
				}
				if (textTag.equals_ignore_case(xml->getNodeName())) {
					std::string txt = xml->getAttributeValueSafe("text");
					if (!txt.empty()) m_allNodes[currentNodeId].setText(wstr(txt));
				}
				if (choiceTag.equals_ignore_case(xml->getNodeName())) {
					m_allNodes[currentNodeId].addChoice(m_parseChoiceInfo(xml));
				}
				//speaker, text, choices
			}
			break;
		case EXN_ELEMENT_END:
			//make sure this doesn't break
			if (current == speakerListTag) current = treeTag;
			else if (current == nodeTag) {
				currentNodeId = L"";
				current = treeTag;
			}
			else current = L"";
			break;
		default:
			break;
		}
	}
	delete xml;
}

bool DialogueTree::m_parseTreeInfo(IrrXMLReader* xml)
{
	std::string id(xml->getAttributeValueSafe("id"));
	std::string minsec(xml->getAttributeValueSafe("minsector"));
	if (id.empty()) {
		std::cout << "Failed to get ID for this dialogue tree!\n";
		return false;
	}
	m_id = wstr(id);
	if (minsec.empty()) {
		std::cout << "Failed to get min sector. Defaulting to 0.\n";
		m_minSector = 0;
	}
	else {
		m_minSector = std::stoi(minsec);
	}
	return true;
}
bool DialogueTree::m_parseNodeInfo(IrrXMLReader* xml)
{
	return true;
}
DialogueChoice DialogueTree::m_parseChoiceInfo(IrrXMLReader* xml)
{
	std::string strtext(xml->getAttributeValueSafe("text"));
	std::string strnext(xml->getAttributeValueSafe("next"));
	std::string streffect(xml->getAttributeValueSafe("effect"));
	return DialogueChoice(wstr(strtext), dialogueEffects.at(wstr(streffect)), wstr(strnext));
}

void DialogueNode::print()
{
	std::wcout << L"Node: " << m_id << L" | Speaker: " << m_speaker << "\n Text: " << m_text << L"\n";
	for (auto choice : m_choices) {
		std::wcout << L"CHOICE: " << choice.text() << L" | NEXT NODE: " << choice.next() << L"\n";
	}
	std::wcout << L"\n";
}

void DialogueTree::print()
{
	std::wcout << L"Tree: " << m_id << L" | Minimum sector: " << m_minSector << L"\n";
	std::wcout << L"Speakers: ";
	for (auto speaker : m_speakers) {
		std::wcout << speaker << L", ";
	}
	std::wcout << L"\n";
	for (auto [id, node] : m_allNodes) {
		node.print();
	}
	std::wcout << L"\n";
}