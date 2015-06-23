#ifndef INCLUDE_GLSCOPEGUI_H
#define INCLUDE_GLSCOPEGUI_H

#include <QWidget>
#include "dsp/dsptypes.h"
#include "util/export.h"
#include "util/message.h"

namespace Ui {
	class GLScopeGUI;
}

class MessageQueue;
class ScopeVis;
class GLScope;

class SDRANGELOVE_API GLScopeGUI : public QWidget {
	Q_OBJECT

public:
	explicit GLScopeGUI(QWidget* parent = NULL);
	~GLScopeGUI();

	void setBuddies(MessageQueue* messageQueue, ScopeVis* scopeVis, GLScope* glScope);

	void resetToDefaults();
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);

	bool handleMessage(Message* message);

private:
	Ui::GLScopeGUI* ui;

	MessageQueue* m_messageQueue;
	ScopeVis* m_scopeVis;
	GLScope* m_glScope;

	int m_sampleRate;

	qint32 m_displayData;
	qint32 m_displayOrientation;
	qint32 m_timeBase;
	qint32 m_timeOffset;
	qint32 m_amplification;

	void applySettings();

private slots:
	void on_amp_valueChanged(int value);
	void on_scope_traceSizeChanged(int value);
	void on_time_valueChanged(int value);
	void on_timeOfs_valueChanged(int value);
	void on_dataMode_currentIndexChanged(int index);

	void on_horizView_clicked();
	void on_vertView_clicked();
};

#endif // INCLUDE_GLSCOPEGUI_H