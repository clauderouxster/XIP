#!/usr/bin/python
"""
Python library/class for working with Google Calendar API.
API docs at: http://code.google.com/apis/gdata/common-elements.html

Do you need to specify your time zone offset to use this?  yes?
Use http://www.isc.tamu.edu/~astro/jupiter/offset.html to find yours.

My Home:
http://code.google.com/p/pygooglecalendar/

Please maintain me ..........!!!!!
"""
import OpenSSL.SSL
import urllib
import urllib2
import httplib
import datetime
import time
import xml.dom.minidom

class SmartRedirectHandler(urllib2.HTTPRedirectHandler):     
    def http_error_301(self, req, fp, code, msg, headers):  
        result = urllib2.HTTPRedirectHandler.http_error_301( 
            self, req, fp, code, msg, headers)              
        result.status = code                                 
        return result                                       
    def http_error_201(self,req,fp,code,msg,headers):
        print "this is OOOK"
        return 1
    def http_error_302(self, req, fp, code, msg, headers):   
        result = urllib2.HTTPRedirectHandler.http_error_302(
            self, req, fp, code, msg, headers)              
        result.status = code                                
        gsessionid = headers.dict['location'].split('?')[1].split('=')[1] 
        return gsessionid
class googCal:
    global gsessionid
    sid = ""
    lsid = ""
    auth = ""
    gses=""
    email=""
    defaultcalendar="https://www.google.com/calendar/feeds/default/private/full"
    time_zone_offset=-4 #not sure if I need this
    def convertTime(self,datetime_obj,ignore_time=False):
        """Convert Python date or datetime object to xs:date or xs:dateTime
        respectively. (don't do offsets yet)
        http://tinyurl.com/nx62o gives up this ref info:
        xs:date - an ISO 8601 date:
        A prefix of the minus sign ('-') mean "BC" (before common era).
        The common format is 'YYYY-MM-DD'.
        xs:dateTime - an ISO 8601 date plus a time.
        The date is separated from the time by a 'T': 2005-03-10T02:00:00-08:00
        The time format is 'hh:mm:ss' with at optional fractional seconds and optional timezone offset.
        The timezone is Greenwich Mean Time unless you specify the offset.
        """
        datestr=str(datetime_obj)
        if ' ' in datestr and ignore_time:datestr=datestr.split(' ')[0]
        if ' ' in datestr:           
            return datestr.replace(' ','T')+"-%s:00" % str(-self.time_zone_offset).zfill(2)
        else:
            #is a date
            return datestr+'T00:00:00' # ? +"-%s:00" % str(-time_zone_offset).zfill(2)
    def getAuth(self,email,password):
        url = "https://www.google.com/accounts/ClientLogin?Email=%s&Passwd=%s&source=essai-e-essai&service=cl" % (email, password)
        #url="https://www.google.com/calendar/render"
        #proxy_support=urllib2._parse_proxy("http://xww.xrce.xerox.com/proxy.pac")
        #proxy_support=urllib2.ProxyHandler({"http":"http://cornillon.grenoble.xrce.xerox.com:8000"})
        #proxy_support = urllib2.ProxyHandler({"http" : "http://ahad-haam:3128"})
        #opener = urllib2.build_opener(proxy_support)
        #urllib2.install_opener(opener)
        #data=urllib2.urlopen("https://www.google.com")
        #proxy_support = urllib2.ProxyHandler({})
        #opener = urllib2.build_opener(proxy_support)
        #urllib2.install_opener(opener)
        ctx = OpenSSL.SSL.Context(OpenSSL.SSL.SSLv23_METHOD)
        ctx.set_options(OpenSSL.SSL.OP_NO_SSLv3)
        
        #x= httplib.HTTPSConnection(port=8000,host="cornillon")
        #x.connect()
        
        print url
        req = urllib2.Request(url)
        req.set_proxy("cornillon.grenoble.xrce.xerox.com:8000","https")
        data = urllib2.urlopen(req).read().split('\n')
        #data = urllib2.urlopen(url).read().split('\n')
        self.sid  = data[0].split('=')[1]
        self.lsid = data[1].split('=')[1]
        self.auth = data[2].split('=')[1]
        self.email = email
        print "Authorized" 
    #TODO: make alias for "login" like add_event so people know how to add event... or just
    #rename login?
    def __make_reminder_element(self,reminder_min_before):
        """Make reminder element to be instered inside gdwhen tags
        See: http://code.google.com/apis/gdata/common-elements.html#gdReminder
        TODO: Add those other attributes, then rename function."""
        return """<gd:reminder minutes="%s"/>""" % reminder_min_before  
     
    def test_and_convert(self,adate,ignore_time=False):
        """datetime object conversions for convienince"""
        if type(adate)==datetime.datetime or type(adate)==datetime.date:                
            return self.convertTime(adate,ignore_time=ignore_time)
        else: #just a string
            return adate #as is        
        
    def login(self,title,content,authorName,authorEmail,where,start,end='',all_day_event=False,reminder_min_before=0):
        """
        Actually makes an event, not sure why this is called "login"?
        DateTime formats expected to be YYYY-MM-DD or YYYY-MM-DDT17:00:00-08:00  
        If starttime is only sent as a date and with no end time, it is an all day event.
        http://code.google.com/apis/gdata/common-elements.html#gdWhen
        
        TODO: add reccurence (easier to just call this multiple times with each
        different date for now.
        http://code.google.com/apis/gdata/common-elements.html#gdRecurrence    
        """
        #Convert potential datetime objects into whatever junk xml needs
        start=self.test_and_convert(start,ignore_time=all_day_event)
        end=self.test_and_convert(end,ignore_time=all_day_event)        
        #make reminder part
        if reminder_min_before:
            reminder=self.__make_reminder_element(reminder_min_before)
        else:
            reminder=''        
        #make date part
        if all_day_event:
            if 'T' in start:start=start.split('T')[0]
            gdwhen="""<gd:when startTime='%s'>%s</gd:when>""" % (start,reminder)
        else:
            gdwhen="""<gd:when startTime='%s'
    endTime='%s'>%s</gd:when>""" % (start,end,reminder)
        authString = "GoogleLogin auth=%s" % (self.auth)
        authurl = self.defaultcalendar
        txData = """<entry xmlns='http://www.w3.org/2005/Atom'
    xmlns:gd='http://schemas.google.com/g/2005'>
  <category scheme='http://schemas.google.com/g/2005#kind'
    term='http://schemas.google.com/g/2005#event'></category>
  <title type='text'>%s</title>
  <content type='text'>%s</content>
  <author>
    <name>%s</name>
    <email>%s</email>
  </author>
  <gd:transparency
    value='http://schemas.google.com/g/2005#event.opaque'>
  </gd:transparency>
  <gd:eventStatus
    value='http://schemas.google.com/g/2005#event.confirmed'>
  </gd:eventStatus>
  <gd:where valueString='%s'></gd:where>
    %s
</entry>""" % (title,content,authorName,authorEmail,where,gdwhen)
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml'
                }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(authurl, txData, txHeaders)

        opener = urllib2.build_opener(SmartRedirectHandler)
        #TODO: make this into a function so I DRY
        try:
            self.gses = opener.open(req)
        except (urllib2.HTTPError,urllib2.URLError):
            #See http://groups.google.com/group/google-calendar-help-dataapi/browse_thread/thread/e135de85c795febe/d3ddd4206cef321e
            #for more info on this issue :-(
            print 'hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print authurl
            time.sleep(25)
            try:
                self.gses = opener.open(req)
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'trying just one more time to recover!'
                time.sleep(240)
                self.gses = opener.open(req)
        try:
            req = urllib2.Request(authurl + "?gsessionid=" + self.gses , txData,txHeaders)
        except TypeError:
            print 'Wierd type error' #I have no clue on why this happens
            print authurl
            print self.gses
            req = urllib2.Request(str(authurl) + "?gsessionid=" + str(self.gses) , txData,txHeaders)        
        #TODO: make this into a function so I DRY
        try:
            data = opener.open(req)
        except urllib2.URLError:
            print 'hit a different crappy google error, please standby while I try to recover'
            print uri
            print req
            #this actually works most of the time
            time.sleep(60)
            data = opener.open(req)

    def query(self,google_type_syntax_search_query_string,startmin='',startmax=''):
        """
        Handle queries made of the calendar working off of the spec here:
        http://code.google.com/apis/gdata/protocol.html#query-requests
        TODO: Allow other queries param.s what the flip is category??
        """
        authString = "GoogleLogin auth=%s" % (self.auth)
        authurl = self.defaultcalendar
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml'
                }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(authurl, '', txHeaders)
        opener = urllib2.build_opener(SmartRedirectHandler)
        try:
            self.gses = opener.open(req)
        except (urllib2.HTTPError,urllib2.URLError):
            print 'hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print authurl
            time.sleep(25)
            try:
                self.gses = opener.open(req)
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'trying just one more time to recover!'
                time.sleep(240)
                self.gses = opener.open(req)                
        #start-min	start-max
        if startmin and startmax:
            start=self.test_and_convert(startmin,ignore_time=False)        
            end=self.test_and_convert(startmax,ignore_time=False)              
            uri_parms_string=urllib.urlencode(
                {'gsessionid':self.gses,
                'q':google_type_syntax_search_query_string,
                'start-min':start,
                'start-max':end,
                'max-results':999999})
        else:
            uri_parms_string=urllib.urlencode(
                {'gsessionid':self.gses,
                'q':google_type_syntax_search_query_string,
                'max-results':999999})        
        uri=authurl + "?" + uri_parms_string
        #print uri
        req = urllib2.Request(url=uri,headers=txHeaders) #don't specify data so it makes GET request.
        try:
            data = opener.open(req).read()
        except urllib2.URLError:
            print 'hit a different crappy google error, please standby while I try to recover'
            print uri
            print req
            #We could try to recover here?
            time.sleep(60)
            data = opener.open(req).read()        
        #print data
        u = unicode(data,"utf-8" )#should already be encoded as this but minidom throws errors for me
        data = u.encode("utf-8")
        #print
        #now parse XML
        #dresults=feedparser.parse(data) #works but doesn't get attributes like GDWhen whatever
        #import pprint
        #print pprint.pformat(dresults)
        def parse_entries(raw_xml):
            dom = xml.dom.minidom.parseString(data)
            #print dom.toprettyxml().encode("utf-8")
            #pull out all entry 's
            entries=dom.getElementsByTagName('entry')
            result_entries=[]
            for entry in entries:
                dentry={}
                dentry['id']=entry.getElementsByTagName('id')[0].firstChild.data
                dentry['published']=entry.getElementsByTagName('published')[0].firstChild.data
                dentry['updated']=entry.getElementsByTagName('updated')[0].firstChild.data
                dentry['title']=entry.getElementsByTagName('title')[0].firstChild.data
                try: dentry['content']=entry.getElementsByTagName('content')[0].firstChild.data
                except AttributeError: dentry['content']=''
                dentry['startTime']=entry.getElementsByTagName('gd:when')[0].getAttribute('startTime')
                dentry['endTime']=entry.getElementsByTagName('gd:when')[0].getAttribute('endTime')
                dentry['location']=entry.getElementsByTagName('gd:where')[0].getAttribute('valueString')
                result_entries.append(dentry)
            return result_entries
        return parse_entries(data)

    def deleteevent(self,publicurl):
        """
        Delete the specified event
	added by David Hautbois - david.hautbois@free.fr  
        """
        txData = ''
        authString = "GoogleLogin auth=%s" % (self.auth)
        authurl = self.getediturl(publicurl)
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml',
            'X-HTTP-Method-Override': 'DELETE'
                }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(authurl, '', txHeaders)
        opener = urllib2.build_opener(SmartRedirectHandler)
        #TODO: make this into a function so I DRY
        try:
            self.gses = opener.open(req)
        except (urllib2.HTTPError,urllib2.URLError):
            #See http://groups.google.com/group/google-calendar-help-dataapi/browse_thread/thread/e135de85c795febe/d3ddd4206cef321e
            #for more info on this issue :-(
            print 'hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print authurl
            time.sleep(25)
            try:
                self.gses = opener.open(req)
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'trying just one more time to recover!'
                time.sleep(240)
                self.gses = opener.open(req)
        try:
            req = urllib2.Request(authurl + "?gsessionid=" + self.gses , txData,txHeaders)
        except TypeError:
            print 'Wierd type error' #I have no clue on why this happens
            print authurl
            print self.gses
            req = urllib2.Request(str(authurl) + "?gsessionid=" + str(self.gses) , txData,txHeaders)        
        #TODO: make this into a function so I DRY
        try:
            data = opener.open(req)
        except urllib2.URLError:
            print 'hit a different crappy google error, please standby while I try to recover'
            #print uri
            print req
            #this actually works most of the time
            time.sleep(60)
            data = opener.open(req)


    def getediturl(self,publicurl):
        """
        added by David Hautbois - david.hautbois@free.fr
        Get the edit ID from an event ID
        The edit ID is nessecary to delete an event
        """
        authString = "GoogleLogin auth=%s" % (self.auth)
        authurl = "https://www.google.com/calendar/feeds/default/private/full"
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml',
            'Debug': 'getediturl'
            }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(authurl, '', txHeaders)
        opener = urllib2.build_opener(SmartRedirectHandler)
        try:
            self.gses = opener.open(req)
        except (urllib2.HTTPError,urllib2.URLError):
            print 'hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print authurl
            time.sleep(25)
            try:
                self.gses = opener.open(req)
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'trying just one more time to recover!'
                time.sleep(240)
                self.gses = opener.open(req)                

        req = urllib2.Request(url=publicurl,headers=txHeaders) #don't specify data so it makes GET request.
        try:
            data = opener.open(req).read()
        except urllib2.URLError:
            print 'hit a different crappy google error, please standby while I try to recover'
            print req
            #We could try to recover here?
            time.sleep(60)
            data = opener.open(req).read()        
        #print data
        u = unicode(data,"utf-8" )#should already be encoded as this but minidom throws errors for me
        data = u.encode("utf-8")
	#print data
        #now parse XML
        #dresults=feedparser.parse(data) #works but doesn't get attributes like GDWhen whatever
        #import pprint
        #print pprint.pformat(dresults)
        dom = xml.dom.minidom.parseString(data)
        #print dom.toprettyxml().encode("utf-8")
        #pull out all entry 's
        entries=dom.getElementsByTagName('entry')
        result_entries=[]
        for entry in entries:
                editurl=entry.getElementsByTagName('link')[2].attributes["href"].value
        #Replace http by https
        return "https" + editurl.lstrip('http')

    def getcalendars(self):
        """
        Added by David Hautbois - david.hautbois@free.fr
        Get the list of the user's calendar
        Return the id, the private URL and the access level
        """

        print "Entering in getcalendars method"
        authString = "GoogleLogin auth=%s" % (self.auth)
        authurl = "https://www.google.com/calendar/feeds/" + self.email
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml'
                }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(authurl, '', txHeaders)
        opener = urllib2.build_opener(SmartRedirectHandler)
        try:
            self.gses = opener.open(req)
            print req._Request__original
        except (urllib2.HTTPError,urllib2.URLError):
            print 'getcalendars : hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print authurl
            time.sleep(10)
            try:
                self.gses = opener.open(req)
                print self.gses
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'getcalendars : trying just one more time to recover!'
                time.sleep(10)
                self.gses = opener.open(req)
       
        req = urllib2.Request(url=authurl + "?gsessionid=" + self.gses,headers=txHeaders) #don't specify data so it makes GET request.
        try:
            data = opener.open(req).read()
        except urllib2.URLError:
            print 'getcalendars : hit a different crappy google error, please standby while I try to recover'
            print uri
            print req
            #We could try to recover here?
            #time.sleep(10)
            data = opener.open(req).read()
        #print data
        u = unicode(data,"utf-8" )#should already be encoded as this but minidom throws errors for me
        data = u.encode("utf-8")
        #print
        #now parse XML
        #dresults=feedparser.parse(data) #works but doesn't get attributes like GDWhen whatever
        #import pprint
        #print pprint.pformat(dresults)
        def parse_entries(raw_xml):
            dom = xml.dom.minidom.parseString(data)
            #print dom.toprettyxml().encode("utf-8")
            #pull out all entry 's
            entries=dom.getElementsByTagName('entry')
            result_entries=[]
            for entry in entries:
                dentry={}
                dentry['title']=entry.getElementsByTagName('title')[0].firstChild.data
                dentry['id']=entry.getElementsByTagName('id')[0].firstChild.data
                dentry['accesslevel']=entry.getElementsByTagName('gCal:accesslevel')[0].attributes["value"].value
                result_entries.append(dentry)
            return result_entries
        return parse_entries(data)    

    def setdefaultcalendar(self,calendarid):
        self.defaultcalendar = "https://www.google.com/calendar/feeds" + calendarid[calendarid.rfind("/"):] + "/private/full"

#--------------------------------------------------------------------------------------------------------------------------
    def updateevent(self,publicurl,title,content,authorName,authorEmail,where,start,end='',all_day_event=False,reminder_min_before=0):
        """
        Actually update an event
        DateTime formats expected to be YYYY-MM-DD or YYYY-MM-DDT17:00:00-08:00  
        If starttime is only sent as a date and with no end time, it is an all day event.
        http://code.google.com/apis/gdata/common-elements.html#gdWhen
        """
        editurl = self.getediturl(publicurl)
        authString = "GoogleLogin auth=%s" % (self.auth)
        #Convert potential datetime objects into whatever junk xml needs
        start=self.test_and_convert(start,ignore_time=all_day_event)
        end=self.test_and_convert(end,ignore_time=all_day_event)        
        #make reminder part
        if reminder_min_before:
            reminder=self.__make_reminder_element(reminder_min_before)
        else:
            reminder=''        
        #make date part
        if all_day_event:
            if 'T' in start:start=start.split('T')[0]
            gdwhen="""<gd:when startTime='%s'>%s</gd:when>""" % (start,reminder)
        else:
            gdwhen="""<gd:when startTime='%s'
    endTime='%s'>%s</gd:when>""" % (start,end,reminder)
        txData = """<entry xmlns='http://www.w3.org/2005/Atom'
    xmlns:gd='http://schemas.google.com/g/2005'>
  <category scheme='http://schemas.google.com/g/2005#kind'
    term='http://schemas.google.com/g/2005#event'></category>
    <id>%s</id>
  <link rel="edit" href="%s"/>
  <title type='text'>%s</title>
  <content type='text'>%s</content>
  <author>
    <name>%s</name>
    <email>%s</email>
  </author>
  <gd:where valueString='%s'></gd:where>
    %s
</entry>""" % (publicurl,editurl,title,content,authorName,authorEmail,where,gdwhen)
        txHeaders = {
            'User-Agent': 'mine',
            'Authorization': authString,
            'Content-Type': 'application/atom+xml',
            'X-HTTP-Method-Override': 'PUT',
            'Debug': 'update'
                }
        httplib.HTTPConnection.debuglevel = 1 #!
        req = urllib2.Request(editurl, txData, txHeaders)
        opener = urllib2.build_opener(SmartRedirectHandler)
        #TODO: make this into a function so I DRY
        try:
            self.gses = opener.open(req)
        except (urllib2.HTTPError,urllib2.URLError):
            #See http://groups.google.com/group/google-calendar-help-dataapi/browse_thread/thread/e135de85c795febe/d3ddd4206cef321e
            #for more info on this issue :-(
            print 'hit a crappy google error, please standby while I try to recover'
            #debug:
            print req._Request__original
            print req.data
            print req.headers
            print editurl
            time.sleep(25)
            try:
                self.gses = opener.open(req)
            except (urllib2.HTTPError,urllib2.URLError):                
                print 'trying just one more time to recover!'
                time.sleep(240)
                self.gses = opener.open(req)
        try:
            req = urllib2.Request(editurl + "?gsessionid=" + self.gses + "&request=1", txData,txHeaders)
        except TypeError:
            print 'Wierd type error' #I have no clue on why this happens
            print editurl
            print self.gses
            req = urllib2.Request(str(editurl) + "?gsessionid=" + str(self.gses), txData,txHeaders)        
        #TODO: make this into a function so I DRY
        try:
            data = opener.open(req)
        except urllib2.URLError:
            print 'hit a different crappy google error, please standby while I try to recover'
            print editurl
            print req
            #this actually works most of the time
            time.sleep(60)
            data = opener.open(req)

        
#if __name__=='__main__':7
    #a = googCal()
    #a.getAuth('username@gmail.com','password')
    #queries=["mom's birthday","birthday","move"]
    #for query in queries:
    #    print a.query(query)
    #test with dates
    #for query in queries:
        #start=datetime.datetime(2006,9,20,0,0,0)
        #end=datetime.datetime(2006,9,20,23,59,59)    
        #print query    
        #print a.query(query,startmin=start,startmax=end) #search all day?
        #print
        
    #a = googCal()
    #a.getAuth('user@gmail.com','password')
    #print all events
    #print a.query("*")
    #Delete the event
    #a.deleteevent("http://www.google.com/calendar/feeds/5mmu2823ffcungf7ef6lstdv88%40group.calendar.google.com/private/full/hpp8lu2tamc4fhba2mbjajdkhk")
    #print a.getcalendars()
    #a.setdefaultcalendar("http://www.google.com/calendar/feeds/david.hautbois%40gmail.com/5mmu2823ffcungf7ef6lstdv88%40group.calendar.google.com")
    #print a.query("*")
    #print a.query("title")
    #print a.getediturl("http://www.google.com/calendar/feeds/5mmu6793ffcungf7ef6lstdv88%40group.calendar.google.com/private/full/hpp8lu2tamc4fhba2mbjajdkhk")
    #a.login("title","content","David","username@gmail.com","location","2007-03-20T20:00:00","2007-03-20T23:00:00")
    #print a.getediturl("http://www.google.com/calendar/feeds/5mmu2393ffcungf7ef6lstdv55%40group.calendar.google.com/private/full/1uj1kg26dkg17es10a1m7lr2rc")
    #myevent="http://www.google.com/calendar/feeds/5mmu2893ffcungf7ef6lstdv55%40group.calendar.google.com/private/full/1uj1kg26dkg17es10a1m7lr2rc"
    #a.updateevent(myevent,"title1","aaa","David","david.hautbois@gmail.com","location","2007-03-19T20:00:00","2007-03-19T21:00:00")
