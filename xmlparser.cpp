#include "xmlparser.h"
#include <QDebug>



XmlParser::XmlParser()
{

}

void XmlParser::setEventList(QStringList xml)
{
    for (int i=0;i<xml.size();++i) {
        QPair<QString, int> singleQuery = getDnsDataFromXml(xml.at(i));
        DnsMetaData *metaData;
        if(dnsData.contains(singleQuery.first)){
            metaData=dnsData.value(singleQuery.first);
            metaData->queries++;
            metaData->process.insert(singleQuery.second);
        }else {
            metaData = new DnsMetaData();
            metaData->queries++;
            metaData->process.insert(singleQuery.second);
            dnsData.insert(singleQuery.first,metaData);
        }
    }
    ordering(parsingQueryToRead(dnsData));

}

QHash<QString , int> XmlParser::parsingQueryToRead(QHash<QString, DnsMetaData*> hash)
{
    QHash<QString , int> dnsResult;
    QHashIterator<QString, DnsMetaData*> i(hash);
    while (i.hasNext()) {
        i.next();
        QString queryName = i.key()+" Process: "+QString::number(i.value()->process.size());

        dnsResult.insert(queryName,i.value()->queries);
    }
    return dnsResult;
}

QPair<QString, int > XmlParser::getDnsDataFromXml(QString xml)
{
//    qDebug()<<"+*+*+*+* "<< xml;
    QPair<QString, int> dnsQuery;
    dnsQuery.second=0;
    QXmlStreamReader reader(xml);
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
            if (reader.name() == "Data") {
                QXmlStreamAttributes attrs = reader.attributes();
                if(attrs.value("Name").toString()=="QueryName"){
                    attrs.value("Name").toString();
                    dnsQuery.first=reader.readElementText();
                }else if (attrs.value("Name").toString()=="ProcessId") {
                    attrs.value("Name").toString();
                    dnsQuery.second= reader.readElementText().toInt();
                }
            }
            continue;
        }
    }
    return dnsQuery;
}

QList<QPair<int, QString>> XmlParser::ordering(QHash<QString , int> result)
{
    QList<QPair<int, QString>> inv;

    // Populate the inverted list
    QHashIterator<QString , int> i(result);
    while(i.hasNext()){
        i.next();
        inv.append(QPair<int, QString>(i.value(), i.key()));
    }
    std::sort(std::begin(inv), std::end(inv));
    int count = 0;
    for (int j = inv.size()-1; j>=0;--j) {
        qDebug() << "Domain: "<<inv.at(j).second << "Queries "<<inv.at(j).first;
        count++;
        if(count==10){
            break;
        }
    }
    return inv;
}



