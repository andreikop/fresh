#ifndef PCONSOLECOMMAND_H
#define PCONSOLECOMMAND_H

#include "core/FreshExport.h"

#include <QStringList>
#include <QHash>

class FRESH_EXPORT pConsoleCommand
{
public:
	enum ExitCode {
		NotFound = -2,
		NoExitCode = -1,
		Success = 0,
		Error = 1
	};
	
	typedef QList<pConsoleCommand*> List;
	
	pConsoleCommand( const QStringList& commands = QStringList() );
	virtual ~pConsoleCommand();

	pConsoleCommand( const pConsoleCommand& other );
	pConsoleCommand& operator=( const pConsoleCommand& other );
	bool operator==( const pConsoleCommand& other ) const;
	bool operator!=( const pConsoleCommand& other ) const;

	QStringList commands() const;
	QStringList autoCompleteList( const QString& command ) const;
	
	virtual QString help( const QString& command ) const;
	void setHelp( const QString& command, const QString& help );

	virtual bool isComplete( const QString& command ) const;
	virtual QString usage( const QString& command ) const;
	virtual QString interpret( const QString& command, int* exitCode ) const;
	
	static QStringList quotedStringList( const QStringList& list );
	static QStringList parseCommand( const QString& command );

protected:
	QStringList mCommands;
	QHash<QString, QString> mHelps;
};

#endif // PCONSOLECOMMAND_H