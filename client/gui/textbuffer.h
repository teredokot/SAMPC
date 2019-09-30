#pragma once

#include "common.h"
#include "textbufferbase.h"

namespace GUI
{
	
	// Defines a basic single color multiline text buffer with caret display capability
	class CTextBuffer :
		public CTextBufferBase
	{
	protected:

		uint m_uiColor;
		char *m_szBuffer;

		int m_iTextStartPos;

		bool m_bCaretEnable;
		uint m_uiCaretColor;
		uint m_uiCaretPosition;

		virtual void UpdateVertexBuffer();

	public:
		CTextBuffer(uint uiMaxChars);
		virtual ~CTextBuffer();

		void AddText(const byte nChar);
		void AddText(const char *szText);
		void InsertText(uint uiPosition, const byte nChar);
		void InsertText(uint uiPosition, const char *szText);
		void RemoveText(uint uiPosition, uint uiLength);

		void ClearText();

		uint GetLength() const;
		const char *GetText() const { return m_szBuffer; }

		void PrintF(const char *szFormat, ...);

		int GetTextStart() const { return m_iTextStartPos; }

		void SetColor(uint color);
		void SetTextStart(int startPos); 

		void SetCaretEnable(bool enabled);
		void SetCaretColor(uint color);
		void SetCaretPosition(int pos);

	};

}
