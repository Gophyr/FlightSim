#pragma once
#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "BaseHeader.h"
#include <functional>
//Typedef for any sort of function that can be used as part of a dialogue choice. Can be generic or specialized.
typedef std::function<void()> DialogueCallback;

class DialogueChoice
{
	public:
		DialogueChoice() : m_used(false) {}
		DialogueChoice(std::wstring text, DialogueCallback cb, std::wstring next) :
			m_text(text), m_callback(cb), m_next(next), m_used(false) {}

		void callback() { m_callback(); }
		const std::wstring text() { return m_text; }
		const std::wstring next() { return m_next; }
		const bool used() { return m_used; }
		bool setUsed(bool used = true) { m_used = used; }
	private:
		std::wstring m_text;
		DialogueCallback m_callback;
		std::wstring m_next;
		bool m_used;

};

class DialogueNode
{
	public:
		DialogueNode() {}
		DialogueNode(std::wstring id) :
			m_id(id) {}
		DialogueNode(std::wstring id, std::wstring speaker, std::wstring text, std::vector<DialogueChoice>& choices) :
			m_id(id), m_speaker(speaker), m_text(text), m_choices(choices) {}

		const std::wstring speaker() { return m_speaker; }
		const std::wstring text() { return m_text; }
		const std::vector<DialogueChoice>& choices() {return m_choices; }
		void setSpeaker(std::wstring speaker) { m_speaker = speaker; }
		void setText(std::wstring text) { m_text = text; }
		void setId(std::wstring id) { m_id = id; }
		void addChoice(DialogueChoice choice) { m_choices.push_back(choice); }

		void print();
	private:
		std::wstring m_id;
		std::wstring m_speaker;
		std::wstring m_text;
		std::vector<DialogueChoice> m_choices;
};

class DialogueTree
{
	public:
		DialogueTree() {}
		//reads tree from XML file
		DialogueTree(std::string filename);
		const bool isUsed() { return m_used; }
		const u32 minSector() { return m_minSector; }
		const std::vector<std::wstring> speakers() { return m_speakers; }
		const std::wstring dialogueId() { return m_id; }

		const DialogueNode& getNode(std::wstring id) { return m_allNodes[id]; }

		void print();
	private:
		bool m_parseTreeInfo(IrrXMLReader* xml);
		bool m_parseNodeInfo(IrrXMLReader* xml);
		DialogueChoice m_parseChoiceInfo(IrrXMLReader* xml);

		std::wstring m_id;
		bool m_used;
		std::vector<std::wstring> m_speakers;
		std::unordered_map<std::wstring, DialogueNode> m_allNodes; //not accessible from the outside; dialogue should trawl the tree
		u32 m_minSector;
};

#endif 