#include "cmdlineparser.h"

///
/// \brief CmdLineParser::CmdLineParser
///
CmdLineParser::CmdLineParser()
    : QCommandLineParser()
{
    QCommandLineOption helpOption(QStringList() << _help, tr("Displays help on commandline options."));
    addOption(helpOption);

    QCommandLineOption versionOption(QStringList() << _version, tr("Displays version information."));
    addOption(versionOption);

    QCommandLineOption profileOption(QStringList() << _profile, tr("Load settings profile from ini file."), tr("file path"));
    addOption(profileOption);
}
