#!/usr/bin/python

import imaplib
import cal
import re

class exchange:
	messages = []
	calendar = []

	def login(self,server, username,password):
		self.server = imaplib.IMAP4(server)
                self.server.login(username,password)

	def getCalendar(self,since):
		self.server.select("Calendar")
		self.r,data = self.server.search(None,'SINCE', since)
		for num in data[0].split():
                        typ,data = self.server.fetch(num, "RFC822")
			self.messages.append(data[0][1])
		self.parseVcal()

	def parseVcal(self):
		startDate = ""
		endDate   = ""
		Summary   = ""
		calfrom   = ""	

		for message in self.messages:
			for lines in message.split('\n'):
				if lines.find("Thread-Topic") != -1:
					Summary = lines.split(':')[1].strip()	
					Summary = re.sub('&','',Summary)
				if lines.find("DTEND;") != -1:
					endDate = lines.split(';')[1].strip()
				if lines.find("DTSTART;") != -1:
					startDate = lines.split(';')[1].strip()
				if lines.find("ORGANIZER;") != -1:
					calfrom = lines.split(';')[1].strip()

			self.calendar.append( {'summary': Summary, 'startdate': startDate, 'enddate': endDate,'from':  calfrom})

	def MonthDayYearHourMin(self,date):
		tempDate = date.split(':')[1]
		year  = tempDate[0:4]
		month   = tempDate[4:6]
		day = tempDate[6:8]

		tempTime = tempDate.split('T')[1]
		hour = tempTime[0:2]
		min  = tempTime[2:4]

		return (month,day,year,hour,min)


# your google email address
googleLogin      = "clauderouxster@gmail.com"
googlePassword   = "maisoncarre"
#exchangeLogin    = ""
#exchangePassword = ""
#exchangeMailServer = ""

a = cal.googCal()

#e = exchange()
#e.login(exchangeMailServer, exchangeLogin,exchangePassword)
#e.getCalendar('"15-Apr-2006"') # where to start getting entries from need '" "' 

startString = "2007-03-24T20:30:00.000-06:00"
endString   = "2007-03-24T21:30:00.000-06:00"
a.getAuth(googleLogin, googlePassword)
a.login("PREMIER" ,"SECOND" ,googleLogin,googleLogin,'ICI DANS CE CAS',startString,endString)
print "FAIT"

#===============================================================================
#for entry in e.calendar:
# 	print entry['summary']
#	start = e.MonthDayYearHourMin(entry['startdate'])
#	startString = start[2] + "-" + start[0] + "-" + start[1] + "T" + start[3] + ":" + start[4] + ":00.000-06:00"
#
#	end   = e.MonthDayYearHourMin(entry['enddate'])
#	endString   = end[2] + "-" + end[0] + "-" + end[1] + "T" + end[3] + ":" + end[4] + ":00.000-06:00"
#
#	print "Start %s - end %s" % (startString, endString)
#	a.getAuth(googleLogin, googlePassword)
#	a.login(entry['summary'] ,entry['summary'] ,googleLogin,googleLogin,'location',startString,endString)
#===============================================================================
