#include "xmlparser.h"

XmlParser::XmlParser()
{

}

void XmlParser::loadKMLFile(QFile *file)
{
    int order=0;
    QXmlStreamReader reader(file);
    while(!reader.atEnd())
    {
        reader.readNext();

        if (reader.isStartDocument()) {
            continue;
        }

        if (reader.isEndDocument()) {
            break;
        }

        if (reader.isStartElement())
        {
            continue;
        }
    }
}
