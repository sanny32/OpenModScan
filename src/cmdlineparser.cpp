#include "cmdlineparser.h"

///
/// \brief CmdLineParser::CmdLineParser
///
CmdLineParser::CmdLineParser()
    : QCommandLineParser()
{
    QCommandLineOption profileOption(QStringList() << _profile, tr("Load settings profile from ini file."), tr("file path"));
    addOption(profileOption);
}
