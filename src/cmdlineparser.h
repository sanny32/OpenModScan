#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

#include <QCommandLineParser>

///
/// \brief The CmdLineParser class
///
class CmdLineParser : public QObject, public QCommandLineParser
{
    Q_OBJECT

public:
    explicit CmdLineParser();

public:
    static constexpr const char* _profile =  "profile";
};

#endif // CMDLINEPARSER_H
