# DnsEventParser

The current project read event logs created by [sysmon](https://docs.microsoft.com/en-us/sysinternals/downloads/sysmon) tool from Sysinternals. The program captures the logs created by DNS query in a specific time and creates a log to show the top 10 requested domains.
It was built on Qt with no runtime dependencies so you can run the program on Windows 10 environment without any problem.

To test the program be sure there is DNS information on the following windows event path:

```
Aplications and service logs/Microsoft/Windows/sysmon
Event id: 22
```
You need to run it from an elevated command prompt and pass a parameter with the time in seconds that you like to analyze your DNS query like this:

```
DnsEventParser.exe 3600
```
The following is an example of the output:
```
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
```
There you can see the domain followed by the number of processes that try to resolve it and the queries made in total.
