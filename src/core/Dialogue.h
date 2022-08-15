#pragma once
#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "BaseHeader.h"
#include <functional>

//Typedef for any sort of function that can be used as part of a dialogue choice. Can be generic or specialized.
typedef std::function<void()> DialogueCallback;

class DialogueNode;

class DialogueChoice
{
	public:
		DialogueChoice() {}
		DialogueChoice(std::string text, DialogueCallback cb, DialogueNode* next = nullptr) :
			m_text(text), m_callback(cb), m_next(next) {}

		void callback() { m_callback(); }
		const std::string text() { return m_text; }
		const DialogueNode* next() { return m_next; }
	private:
		std::string m_text;
		DialogueCallback m_callback;
		DialogueNode* m_next;

};

class DialogueNode
{
	public:
		DialogueNode() {}
		DialogueNode(std::string speaker, std::string text, std::vector<DialogueChoice>& choices) :
			m_speaker(speaker), m_text(text), m_choices(choices) {}

		const std::string speaker() { return m_speaker; }
		const std::string text() { return m_text; }
		const std::vector<DialogueChoice>& choices() {return m_choices; }
	private:
		std::string m_speaker;
		std::string m_text;
		std::vector<DialogueChoice> m_choices;
};

class DialogueTree
{
	public:
		DialogueTree() {}
		//read a tree from XML should be the constructor here
		DialogueTree(std::string filename);
		~DialogueTree() {
			for (auto node : m_allNodes) {
				delete node;
			}
		}
		const DialogueNode* start() { return m_start; }
		const bool isUsed() { return m_used; }
		const u32 minSector() { return m_minSector; }
		const std::vector<std::string> speakers() { return m_speakers; }
		const std::string dialogueId() { return m_id; }

	private:
		std::string m_id;
		DialogueNode* m_start;
		bool m_used;
		std::vector<std::string> m_speakers;
		std::vector<DialogueNode*> m_allNodes; //not accessible from the outside; dialogue should trawl the tree
		u32 m_minSector;
};

#endif 