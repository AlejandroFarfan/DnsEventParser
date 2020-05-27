# DnsEventParser

The current project read event logs created by sysmon tool from sysinternals, the program capture the logs created by DNS query on a user specific time and create a log to show the top 10 requested domains.
It was build on Qt with no runtime dependencies so you can run the program on windows 10 enviroment without any problem.

To test the program be sure there is dns information on the following windows event path
Aplications and service logs/Microsoft/Windows/sysmon
Event id: 22

You need to run it from an elevated comand prompt with the time in seconds do you like to analyze your dns query like this:

DnsEventParser.exe 3600

The folowing is an example of the output:

Domain:  "github.com Process: 134" Queries  145
Domain:  "btlr.sharethrough.com Process: 1" Queries  145
Domain:  "ads.yieldmo.com Process: 1" Queries  127
Domain:  "tlx.3lift.com Process: 1" Queries  125
Domain:  "prebid.technoratimedia.com Process: 1" Queries  125
Domain:  "hb.emxdgt.com Process: 1" Queries  108
Domain:  "ib.adnxs.com Process: 1" Queries  68
Domain:  "mobile.pipe.aria.microsoft.com Process: 5" Queries  54
Domain:  "outlook.office.com Process: 3" Queries  38
Domain:  "logs.us-east-1.amazonaws.com Process: 2" Queries  32

There you can see the domain following by the amount of process that try to resolve it an the queries made in total.

[3] https://docs.microsoft.com/en-us/sysinternals/downloads/sysmon
