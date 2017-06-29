#ifndef ICONSOLE_H
#define ICONSOLE_H
#include <vector>
#include <sstream>
#include <string>

class IConsole
{
public:
	IConsole()
		: m_bFlushed(true)
		, m_bActive(true)
		, m_bDestroyed(true)
		, m_iTextColor(0) // black
	{}

	virtual ~IConsole() {}

	virtual bool init(const std::string& title = "Debug") = 0;
	
	virtual void flush() = 0;
	
	virtual void writeAndResetBuffer() = 0;

public:
	template <class T>
	IConsole& operator<<(const T& t)
	{
		if (!m_bActive || m_bDestroyed)return *this;

		if (m_vBuffer.size() > MaxBufferSize) {
			writeAndResetBuffer();
		}

		// int t is null ,flush buffer
		std::stringstream ss; ss << t;
		if (ss.str().empty()) { flush(); return *this; }

		if (!m_bFlushed) {
			m_vBuffer.back() += ss.str();
		}
		else {
			m_vBuffer.push_back(ss.str());
			m_bFlushed = false;
		}
		return *this;
	}

	void on() { m_bActive = true; }
	void off() { m_bActive = false; }

	bool isDestroyed() { return m_bDestroyed; }

	void clearBuffer() { m_vBuffer.clear(); flush(); }
	void setTextColor(unsigned int color) { m_iTextColor = color; }
	unsigned int getTextColor() { return m_iTextColor; }
protected:
	bool m_bFlushed;
	bool m_bActive;
	bool m_bDestroyed;

	unsigned int m_iTextColor;

	std::vector<std::string> m_vBuffer;

	static const int MaxBufferSize = 500;
};

#endif