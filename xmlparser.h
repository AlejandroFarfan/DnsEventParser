#ifndef XMLPARSER_H
#define XMLPARSER_H
#include <QXmlStreamReader>
#include <QFile>
#include <QStringList>
#include <QPair>
#include <QList>
#include <QHash>
#include <QSet>

using namespace std;

struct DnsMetaData
{
    DnsMetaData() {queries=0;}
    int queries;
    QSet<int> process;
};

class XmlParser
{
public:
    XmlParser();
    QPair<QString, int> getDnsDataFromXml(QString xml);
    void setEventList(QStringList xml);
    ~XmlParser();

private:
    QHash<QString, DnsMetaData*> dnsData;
    QList<QPair<int, QString>> XmlParser::ordering(QHash<QString, int> result);
    QHash<QString , int> parsingQueryToRead(QHash<QString, DnsMetaData*> hash);

};

#endif // XMLPARSER_H
