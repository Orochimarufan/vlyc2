#!/usr/bin/python3
#-------------------------------------------------------------------------------
#- Copyright (C) 2013  Orochimarufan
#-
#- This program is free software: you can redistribute it and/or modify
#- it under the terms of the GNU General Public License as published by
#- the Free Software Foundation, either version 3 of the License, or
#- (at your option) any later version.
#-
#- This program is distributed in the hope that it will be useful,
#- but WITHOUT ANY WARRANTY; without even the implied warranty of
#- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#- GNU General Public License for more details.
#-
#- You should have received a copy of the GNU General Public License
#- along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

import datetime
import re
import decimal

#------------------------------------------------------------
# ISO8601 DateTime parser
#------------------------------------------------------------
date_regex = (r"(?:"
               r"(?:(?P<sign>[+-])(?P<year5>[0-9]{5,}[0-9]+)|(?P<year>[0-9]{4}))"
               r"(?:(?P<datesep>-?)(?:"
                # y-m-d
                r"(?P<month>[0-9]{2})"
                r"(?:(?P=datesep)(?P<day>[0-9]{2}))?"
               r"|"
                # y-w-d
                r"W(?P<week>[0-9]{2})"
                r"(?:(?P=datesep)(?P<weekday>[0-7]))?"
               r"|"
                # y-d
                r"(?P<ordinal>[0-9]{3})"
               r"))?|"
               # century
               r"(?:(?P<centsign>[+-])(?P<cent3>[0-9]{3,4})|(?P<century>[0-9]{2}))"
              r")")
time_regex = (r"(?P<hour>[0-9]{2})"
              r"(?:(?P<timesep>:?)(?P<minute>[0-9]{2})"
              r"(?:(?P=timesep)(?P<second>[0-9]{2})))"
              r"(?:[,.](?P<fraction>[0-9]+))?")
tz_regex    = r"(?P<tz>(?:Z|(?P<tzsign>[+-])(?P<tzhour>[0-9]{2})(?::?(?P<tzmin>[0-9]{2}))?)?)"
time_re  = re.compile(time_regex + tz_regex)
date_re  = re.compile(date_regex)
datetime_re   = re.compile(date_regex + "T" + time_regex + tz_regex)
zero_timedelta = datetime.timedelta(0)


class TZInfo(datetime.tzinfo):
    """ A TZInfo class that stores plain offsets """
    def __init__(self, offset_hours=0, offset_mins=0, name='UTC'):
        self.__offset = datetime.timedelta(hours=offset_hours, minutes=offset_mins)
        self.__name = name
    
    def utcoffset(self, dt):
        return self.__offset
    
    def tzname(self, dt):
        return self.__name
    
    def dst(self, dt):
        return zero_timedelta
    
    def __repr__(self):
        return '<ISO8601 TZInfo %r>' % self.__name

UTC = TZInfo()


def parse_datetime(s):
    """ parses a ISO8601 DateTime """
    match = datetime_re.match(s)
    if not match:
        raise ValueError("not a valid ISO8601 DateTime: '%s'" % s)
    groups = match.groupdict()
   
    return datetime.datetime.combine(extract_date(groups), extract_time(groups))


def parse_date(s):
    """ parses a ISO8601 Date """
    match = date_re.match(s)
    if not match:
        raise ValueError("not a valid ISO8601 Date: '%s'" % s)
    
    return extract_date(match.groupdict())


def parse_time(s):
    """ parses a ISO8601 Time """
    match = time_re.match(s)
    if not match:
        raise ValueError("not a valid ISO8601 Time: '%s'" % s)
    
    return extract_time(match.groupdict())


def extract_date(groups):
    """ Extracts the date from regexp results """
    # century date
    has = lambda k: k in groups and groups[k] is not None
    if has('cent3'):
        sign = -1 if groups['centsign'] == "-" else 1
        date = datetime.date(sign * (int(groups['century']) * 100 + 1), 1, 1)
    elif has('century'):
        date = datetime.date(int(groups['century']) * 100, 1, 1)
   
    else:
        # year
        if has('year5'):
            year = int(groups['year5']) * -1 if groups['sign'] == "-" else 1
        else:
            year = int(groups['year'])
       
        # y-m-*
        if has('month'):
            if has('day'):
                date = datetime.date(year, int(groups['month']), int(groups['day']))
            else:
                date = datetime.date(year, int(groups['month']), 1)
       
        else:
            date = datetime.date(year, 1, 1)
           
            # y-w-d
            if has('week'):
                iso = date.isocalendar()
               
                date += datetime.timedelta(
                    weeks=int(groups['week']) - (1 if iso[1] == 1 else 0),
                    days=-iso[2] + (int(groups['weekday']) if has('weekday') else 1))
           
            # y-d
            elif has('ordinal'):
                date += datetime.timedelta(days=int(groups['ordinal']) - 1)
   
    return date


def extract_time(groups):
    """ Extracts the time from regexp results """
    has = lambda k: k in groups and groups[k] is not None
    get = lambda k, d: groups[k] if has(k) else d
   
    # TZInfo
    if not groups['tz']:
        tz = None
    elif groups['tz'] == 'Z':
        tz = UTC
    else:
        sign = -1 if groups['tzsign'] == '-' else 1
        tz = TZInfo(sign * int(get('tzhour', 0)),
                            sign * int(get('tzmin', 0)),
                            groups['tz'])
   
    # Time
    if has('fraction'):
        fraction = decimal.Decimal('0.' + groups['fraction'])
    else:
        fraction = 0
   
    if has('second'):
        time = datetime.time(int(groups['hour']), int(groups['minute']), int(groups['second']),
            (fraction.quantize(decimal.Decimal('.000001')) * int(1e6)).to_integral(), tz)
    elif has('minute'):
        second = fraction * 60
        time = datetime.time(int(groups['hour']), int(groups['minute']), int(second),
            ((second - int(second)).quantize(decimal.Decimal('0.000001')) * int(1e6)).to_integral(), tz)
    else:
        minute = fraction * 60
        second = (minute - int(minute)) * 60
        time = datetime.time(int(groups['hour']), int(minute), int(second),
            ((second - int(second)).quantize(decimal.Decimal('0.000001')) * int(1e6)).to_integral(), tz)
   
    return time
