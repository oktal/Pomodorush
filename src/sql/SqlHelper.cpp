#include "SqlHelper.h"
#include "parser/tosqlparse.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

void printStatement( toSQLParse::statement& s, int prof = 0 )
{
    const QString indent = QString( prof, '\t' );
    
    if ( prof == 0 ) {
        qWarning() << "-----------------------------";
    }
    
    qWarning() << qPrintable( (QStringList() << indent << QString::number(s.Type) << s.String).join( " / " ) );
    
    foreach ( toSQLParse::statement t, s.subTokens() ) {
        prof++;
        printStatement( t, prof );
        prof--;
    }
}

bool SqlHelper::openDatabase()
{
	qWarning() << Q_FUNC_INFO << "Using database in" << qPrintable( SqlHelper::databaseLocalFilePath() );
	
	bool open = false;
	
	{
		QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", SqlHelper::connectionName() );
		db.setDatabaseName( SqlHelper::databaseLocalFilePath() );
		open = db.open();
		
		if ( open && db.tables().isEmpty() ) {
			open = SqlHelper::initializeDatabase();
		}
	}
	
	if ( !open ) {
		SqlHelper::closeDatabase();
	}
	
	return open;
}

bool SqlHelper::initializeDatabase()
{
	const bool inTransaction = transaction();
	
	foreach ( const QString& filePath, SqlHelper::initializeScripts() ) {
		if ( !SqlHelper::executeSqlScript( filePath ) ) {
			if ( inTransaction ) {
				rollback();
			}
			
			return false;
		}
	}
	
	if ( inTransaction ) {
		commit();
	}
	
	return true;
}

void SqlHelper::closeDatabase()
{
	{
		QSqlDatabase db = SqlHelper::database();
		
		if ( db.isOpen() ) {
			db.close();
		}
	}
	
	QSqlDatabase::removeDatabase( SqlHelper::connectionName() );
}

bool SqlHelper::transaction()
{
	const bool ok = database().transaction();
	
	if ( !ok ) {
		lastError( QSqlQuery(), Q_FUNC_INFO );
	}
    else {
        qWarning() << Q_FUNC_INFO << "Begin transaction";
    }
	
	return ok;
}

bool SqlHelper::commit()
{
	const bool ok = database().commit();
	
	if ( !ok ) {
		lastError( QSqlQuery(), Q_FUNC_INFO );
	}
    else {
        qWarning() << Q_FUNC_INFO << "Commit transaction";
    }
	
	return ok;
}

bool SqlHelper::rollback()
{
	const bool ok = database().rollback();
	
	if ( !ok ) {
		lastError( QSqlQuery(), Q_FUNC_INFO );
	}
    else {
        qWarning() << Q_FUNC_INFO << "Rollback transaction";
    }
	
	return ok;
}

QStringList SqlHelper::initializeScripts()
{
	const QString path = ":/sql";
	const QStringList entries = QDir( path ).entryList( QStringList( "*.sql" ), QDir::Files | QDir::NoDotDot, QDir::Name );
	QStringList files;
	
	foreach ( const QString& entry, entries ) {
		files << QDir::cleanPath( QString( "%1/%2" ).arg( path ).arg( entry ) );
	}
	
	return files;
}

QString SqlHelper::connectionName()
{
    return QLatin1String( "Pomodorush DB" );
}

QString SqlHelper::databaseLocalFilePath()
{
	const QString path = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
	const QString filePath = QDir::cleanPath( QString( "%1/database.s3db" ).arg( path ) );
	
	if ( !QFile::exists( filePath ) ) {
		if ( !QDir().mkpath( path ) ) {
			qWarning() << Q_FUNC_INFO << "Can't create path" << qPrintable( path );
		}
	}
	
	return filePath;
}

QSqlDatabase SqlHelper::database()
{
	return QSqlDatabase::database( SqlHelper::connectionName(), false );
}

QSqlQuery SqlHelper::query()
{
	return QSqlQuery( SqlHelper::database() );
}

QString SqlHelper::lastError( const QSqlQuery& query, const char* function )
{
	QString error = query.lastError().text();
	
	if ( error.isEmpty() ) {
		error = database().lastError().text();
	}
	
	qWarning() << "********** SQL ERROR **********";
	qWarning() << function;
	qWarning() << qPrintable( error );
	qWarning() << query.boundValues();
	qWarning() << qPrintable( query.lastQuery() );
	qWarning() << qPrintable( query.executedQuery() );
	
	return error;
}

bool SqlHelper::executeSqlScript( const QString& filePath )
{
	qWarning() << Q_FUNC_INFO << "Executing" << qPrintable( filePath );
	QSqlQuery q = SqlHelper::query();
	QFile file( filePath );
	
	if ( !file.open( QIODevice::ReadOnly ) ) {
		qWarning() << Q_FUNC_INFO << "Can't open file" << qPrintable( filePath );
		return false;
	}
	
	toSQLParse::settings settings;
	settings.ExpandSpaces = false;
	settings.CommaBefore = false;
	settings.BlockOpenLine = false;
	settings.OperatorSpace = false;
	settings.KeywordUpper = false;
	settings.RightSeparator = false;
	settings.EndBlockNewline = false;
	settings.IndentLevel = true;
	settings.CommentColumn = false;
	toSQLParse::setSetting( settings );
	
	const QString content = QString::fromUtf8( file.readAll() );
	toSQLParse::stringTokenizer tokens( content );
	int lastOffset = 0;
	int count = 0;
	int skip = 0;
	
	while ( tokens.offset() != content.length() ) {
		toSQLParse::statement statement = toSQLParse::parseStatement( tokens );
		const QString sql = content.mid( lastOffset, tokens.offset() -lastOffset ).trimmed();
		
		//printStatement( statement );
		
		if ( statement.subTokens().size() == 2 ) {
			toSQLParse::statement firstToken = *statement.subTokens().begin();

			if ( firstToken.Type == toSQLParse::statement::Keyword ) {
				if ( firstToken.String == "BEGIN" || firstToken.String == "COMMIT" ) {
					toSQLParse::statement secondToken = *(++statement.subTokens().begin());

					if ( secondToken.Type == toSQLParse::statement::Token ) {
						if ( secondToken.String == ";" ) {
							qWarning() << "*** Skipping" << firstToken.String +";";
							lastOffset = tokens.offset();
							skip++;

							if ( tokens.offset() != content.length() ) {
								continue;
							}
						}
					}
				}
			}
		}
		
		if ( !sql.isEmpty() && !q.exec( sql ) ) {
			lastError( q, Q_FUNC_INFO );
			return false;
		}

		lastOffset = tokens.offset();
		count++;
	}

	qWarning() << Q_FUNC_INFO << "Executed" << count -1 << " statements and skipped" << skip;
	return true;
}
