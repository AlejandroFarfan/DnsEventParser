#ifndef XMLPARSER_H
#define XMLPARSER_H
#include <QXmlStreamReader>
#include <QFile>

class XmlParser
{
public:
    XmlParser();
    void loadKMLFile(QFile *file);
};

#endif // XMLPARSER_H
