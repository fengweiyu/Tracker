#include "Apns.h"

const apns apn_table[] = {
	{"202","01","internet","",""},
	{"202","05","internet","user","pass"},
	{"202","05","internet.vodafone.gr","user","pass"},
	{"202","05","surfonly.vodafone.gr","",""},
	{"202","10","gint.b-online.gr","wap","wap"},
	{"204","04","live.vodafone.com","vodafone","vodafone"},
	{"204","08","portalmmm.nl","",""},
	{"204","12","internet","",""},
	{"204","16","internet","*","*"},
	{"204","20","internet","",""},
	{"204","20","rabo.plus","",""},
	{"206","10","mworld.be","",""},
	{"206","20","gprs.base.be","base","base"},
	{"208","01","orange-mib","mportail","mib"},
	{"208","01","ofnew.fr","orange","orange"},
	{"208","01","orange","orange","orange"},
	{"208","02","orange.fr","",""},
	{"208","10","sl2sfr","",""},
	{"208","10","wapsfr","",""},
	{"208","10","websfr","",""},
	{"208","11","websfr","",""},
	{"208","13","websfr","",""},
	{"208","20","a2bouygtel.com","",""},
	{"208","20","mmsbouygtel.com","",""},
	{"208","21","ebouygtel.com","",""},
	{"208","88","ebouygtel.com","",""},
	{"214","01","airtelwap.es","wap@wap","wap125"},
	{"214","03","orangeworld","orange","orange"},
	{"214","04","internet","",""},
	{"214","06","airtelnet.es","vodafone","vodafone"},
	{"214","07","movistar.es","MOVISTAR","MOVISTAR"},
	{"216","01","net","",""},
	{"216","30","internet","",""},
	{"216","70","vitamax.internet.vodafone.net","",""},
	{"216","70","internet.tescomobile.hu","",""},
	{"219","01","web.htgprs","",""},
	{"222","01","ibox.tim.it","",""},
	{"222","01","web.noverca.it","",""},
	{"222","10","mobile.vodafone.it","",""},
	{"222","10","web.omnitel.it","",""},
	{"222","88","internet.wind","",""},
	{"222","99","apn.fastweb.it","",""},
	{"222","99","tre.it","",""},
	{"226","01","internet.vodafone.ro","internet.vodafone.ro","vodafone"},
	{"226","03","internet","",""},
	{"226","10","internet","",""},
	{"228","01","gprs.swisscom.ch","",""},
	{"228","02","internet","internet","internet"},
	{"228","03","internet","",""},
	{"230","01","internet.t-mobile.cz","wap","wap"},
	{"230","02","internet","",""},
	{"230","03","internet","",""},
	{"230","03","ointernet","",""},
	{"231","02","internet","",""},
	{"231","04","internet","",""},
	{"231","05","internet","",""},
	{"231","06","o2internet","",""},
	{"232","01","a1.net","ppp@a1plus.at","ppp"},
	{"232","03","gprsinternet","t-mobile","tm"},
	{"232","05","orange.web","web","web"},
	{"232","07","web","web@telering.at","web"},
	{"232","10","drei.at","",""},
	{"234","01","internet","",""},
	{"234","02","mobile.o2.co.uk","O2web","O2web"},
	{"234","10","mobile.o2.co.uk","O2web","O2web"},
	{"234","11","mobile.o2.co.uk","O2web","O2web"},
	{"234","15","wap.vodafone.co.uk","wap","wap"},
	{"234","15","internet","web","web"},
	{"234","15","pp.vodafone.co.uk","wap","wap"},
	{"234","20","three.co.uk","",""},
	{"234","30","general.t-mobile.uk","t-mobile","tm"},
	{"234","33","orangeinternet","",""},
	{"234","34","orangeinternet","",""},
	{"234","50","pepper","",""},
	{"234","55","internet","",""},
	{"234","58","3gpronto","",""},
	{"234","58","web.manxpronto.net","gprs","gprs"},
	{"238","01","wap","",""},
	{"238","02","Internet","",""},
	{"238","06","data.tre.dk","",""},
	{"238","20","www.internet.mtelia.dk","",""},
	{"238","77","Internet","",""},
	{"240","01","online.telia.se","",""},
	{"240","017","halebop.telia.se","",""},
	{"240","02","data.tre.se","",""},
	{"240","04","services.telenor.se","",""},
	{"240","06","services.telenor.se","",""},
	{"240","07","internet.tele2.se","",""},
	{"240","08","services.telenor.se","",""},
	{"240","09","services.telenor.se","",""},
	{"240","10","data.springmobil.se","",""},
	{"242","01","telenor","",""},
	{"242","02","netcom","",""},
	{"242","04","internet.tele2.no","",""},
	{"242","05","internet","",""},
	{"244","03","internet","",""},
	{"244","04","internet","",""},
	{"244","05","internet","",""},
	{"244","10","internet.song.fi","song@internet","songnet"},
	{"244","12","internet","",""},
	{"244","13","internet","",""},
	{"244","21","wap.saunalahti.fi","",""},
	{"244","91","wap.sonera.net","",""},
	{"246","01","omnitel","omni","omni"},
	{"246","02","banga","",""},
	{"246","03","internet.tele2.lt","",""},
	{"247","01","internet.lmt.lv","lmt","lmt"},
	{"247","02","internet.tele2.lv","wap","wap"},
	{"247","05","internet","",""},
	{"248","01","internet.emt.ee","",""},
	{"248","03","internet.tele2.ee","",""},
	{"250","01","internet.mts.ru","mts","mts"},
	{"250","02","internet","gdata","gdata"},
	{"250","04","internet.mts.ru","mts","mts"},
	{"250","05","internet.mts.ru","mts","mts"},
	{"250","07","internet.smarts.ru","any","any"},
	{"250","10","internet.mts.ru","mts","mts"},
	{"250","11","internet.beeline.ru","beeline","beeline"},
	{"250","12","inet.bwc.ru","bwc","bwc"},
	{"250","13","internet.mts.ru","mts","mts"},
	{"250","17","internet.usi.ru","",""},
	{"250","20","internet.tele2.ru","",""},
	{"250","20","wap.tele2.ru","",""},
	{"250","39","internet.mts.ru","mts","mts"},
	{"250","44","internet.beeline.ru","beeline","beeline"},
	{"250","92","internet.mts.ru","mts","mts"},
	{"250","93","internet.mts.ru","mts","mts"},
	{"250","99","internet.beeline.ru","beeline","beeline"},
	{"255","01","www.mts.com.ua","",""},
	{"255","02","internet.beeline.ua","",""},
	{"255","03","www.kyivstar.net","igprs","internet"},
	{"255","06","internet","",""},
	{"255","07","3g.utel.ua","",""},
	{"260","01","internet","",""},
	{"260","02","erainternet","erainternet","erainternet"},
	{"260","02","heyah.pl","heyah","heyah"},
	{"260","03","internet","ppp","ppp"},
	{"260","06","internet","",""},
	{"262","01","internet.t-mobile","t-mobile","tm"},
	{"262","02","web.vodafone.de","",""},
	{"262","03","internet.eplus.de","eplus","internet"},
	{"262","03","wap.eplus.de","",""},
	{"262","04","web.vodafone.de","",""},
	{"262","05","internet.eplus.de","eplus","internet"},
	{"262","06","internet.t-mobile","t-mobile","tm"},
	{"262","07","internet","",""},
	{"262","08","internet","",""},
	{"262","09","web.vodafone.de","",""},
	{"262","11","internet","",""},
	{"268","01","net2.vodafone.pt","vodafone","vodafone"},
	{"268","03","umts","",""},
	{"268","06","internet","",""},
	{"272","01","live.vodafone.com","",""},
	{"272","02","internet","",""},
	{"272","03","data.mymeteor.ie","",""},
	{"272","03","mms.eircom.ie","",""},
	{"272","05","3ireland.ie","",""},
	{"272","11","Tescomobile.liffeytelecom.com","",""},
	{"274","01","internet","",""},
	{"274","02","gprs.is","",""},
	{"274","11","net.nova.is","",""},
	{"286","01","internet","",""},
	{"286","02","internet","vodafone","vodafone"},
	{"286","03","internet","",""},
	{"294","01","internet","internet","t-mobile"},
	{"297","02","tmcg-wnw","38267","38267"},
	{"302","220","sp.telus.com","",""},
	{"302","220","sp.koodo.com","",""},
	{"302","370","internet.fido.ca","fido","fido"},
	{"302","610","pda.bell.ca","",""},
	{"302","610","inet.bell.ca","",""},
	{"302","720","rogers-core-appl1.apn","",""},
	{"302","720","internet.com","wapuser1","wap"},
	{"302","370","fido-core-appl1.apn","",""},
	{"302","720","internet.com","guest","guest"},
	{"302","720","rogers-core-appl1.apn","",""},
	{"310","030","private.centennialwireless.com","privuser","priv"},
	{"310","090","isp","",""},
	{"310","100","plateauweb","",""},
	{"310","160","epc.tmobile.com","",""},
	{"310","170","isp.cingular","",""},
	{"310","200","epc.tmobile.com","",""},
	{"310","210","epc.tmobile.com","",""},
	{"310","220","epc.tmobile.com","",""},
	{"310","230","epc.tmobile.com","",""},
	{"310","240","epc.tmobile.com","",""},
	{"310","250","epc.tmobile.com","",""},
	{"310","260","epc.tmobile.com","",""},
	{"310","270","epc.tmobile.com","",""},
	{"310","310","epc.tmobile.com","",""},
	{"310","380","proxy","",""},
	{"310","410","wap.cingular","WAP@CINGULARGPRS.COM","CINGULAR1"},
	{"310","470","isp.cingular","",""},
	{"310","470","wap.cingular","WAP@CINGULARGPRS.COM","CINGULAR1"},
	{"310","480","isp.cingular","",""},
	{"310","480","wap.cingular","WAP@CINGULARGPRS.COM","CINGULAR1"},
	{"310","490","epc.tmobile.com","",""},
	{"310","580","epc.tmobile.com","",""},
	{"310","610","internet.epictouch","",""},
	{"310","660","epc.tmobile.com","",""},
	{"310","770","i2.iwireless.com","",""},
	{"310","800","epc.tmobile.com","",""},
	{"310","840","isp","",""},
	{"310","910","wap.firstcellular.com","",""},
	{"311","210","internet.farmerswireless.com","",""},
	{"334","020","internet.itelcel.com","webgprs","webgprs2002"},
	{"334","02","internet.itelcel.com","webgprs","webgprs2002"},
	{"334","030","internet.movistar.mx","movistar","movistar"},
	{"334","03","internet.movistar.mx","movistar","movistar"},
	{"334","050","modem.iusacellgsm.mx","iusacellgsm","iusacellgsm"},
	{"404","01","www","",""},
	{"404","01","portalnmms","",""},
	{"404","02","airtelgprs.com","",""},
	{"404","03","airtelgprs.com","",""},
	{"404","04","internet","",""},
	{"404","05","www","",""},
	{"404","05","portalnmms","",""},
	{"404","07","internet","",""},
	{"404","09","SMARTNET","",""},
	{"404","10","airtelgprs.com","",""},
	{"404","11","www","",""},
	{"404","11","portalnmms","",""},
	{"404","12","internet","",""},
	{"404","13","www","",""},
	{"404","13","portalnmms","",""},
	{"404","14","spicegprs","",""},
	{"404","15","www","",""},
	{"404","15","portalnmms","",""},
	{"404","16","airtelgprs.com","",""},
	{"404","17","aircelwebpost","",""},
	{"404","18","SMARTNET","",""},
	{"404","19","internet","",""},
	{"404","20","www","",""},
	{"404","20","portalnmms","",""},
	{"404","24","internet","",""},
	{"404","24","imis","",""},
	{"404","25","aircelwebpost","",""},
	{"404","27","www","",""},
	{"404","27","portalnmms","",""},
	{"404","28","aircelwebpost","",""},
	{"404","29","aircelwebpost","",""},
	{"404","30","www","",""},
	{"404","30","portalnmms","",""},
	{"404","31","airtelgprs.com","",""},
	{"404","33","aircelwebpost","",""},
	{"404","34","bsnlnet","",""},
	{"404","35","aircelwebpost","",""},
	{"404","36","SMARTNET","",""},
	{"404","37","aircelwebpost","",""},
	{"404","38","bsnlnet","",""},
	{"404","40","airtelgprs.com","",""},
	{"404","41","aircelgprs.po","",""},
	{"404","42","aircelgprs.po","",""},
	{"404","43","www","",""},
	{"404","43","portalnmms","",""},
	{"404","44","simplyinternet","",""},
	{"404","45","airtelgprs.com","",""},
	{"404","46","www","",""},
	{"404","46","portalnmms","",""},
	{"404","49","airtelgprs.com","",""},
	{"404","50","SMARTNET","",""},
	{"404","51","bsnlnet","",""},
	{"404","52","SMARTNET","",""},
	{"404","53","bsnlnet","",""},
	{"404","54","bsnlnet","",""},
	{"404","55","bsnlnet","",""},
	{"404","56","internet","",""},
	{"404","57","bsnlnet","",""},
	{"404","58","bsnlnet","",""},
	{"404","59","bsnlnet","",""},
	{"404","60","www","",""},
	{"404","60","portalnmms","",""},
	{"404","62","bsnlnet","",""},
	{"404","64","bsnlnet","",""},
	{"404","66","bsnlnet","",""},
	{"404","67","SMARTNET","",""},
	{"404","68","mtnl.net","mtnl","mtnl123"},
	{"404","68","mtnl3g","mtnl","mtnl123"},
	{"404","69","mtnl.net","",""},
	{"404","69","mtnl.net","",""},
	{"404","70","airtelgprs.com","",""},
	{"404","71","bsnlnet","",""},
	{"404","72","bsnlnet","",""},
	{"404","73","bsnlnet","",""},
	{"404","74","bsnlnet","",""},
	{"404","75","bsnlnet","",""},
	{"404","76","bsnlnet","",""},
	{"404","77","bsnlnet","",""},
	{"404","78","internet","",""},
	{"404","79","bsnlnet","",""},
	{"404","80","bsnlnet","",""},
	{"404","81","bsnlnet","",""},
	{"404","82","internet","",""},
	{"404","83","SMARTNET","",""},
	{"404","84","www","",""},
	{"404","84","portalnmms","",""},
	{"404","85","SMARTNET","",""},
	{"404","86","portalnmms","",""},
	{"404","87","internet","",""},
	{"404","88","www","",""},
	{"404","88","portalnmms","",""},
	{"404","89","internet","",""},
	{"404","90","airtelgprs.com","",""},
	{"404","91","aircelwebpost","",""},
	{"404","92","airtelgprs.com","",""},
	{"404","93","airtelgprs.com","",""},
	{"404","94","airtelgprs.com","",""},
	{"404","95","airtelgprs.com","",""},
	{"404","96","airtelgprs.com","",""},
	{"404","97","airtelgprs.com","",""},
	{"404","98","airtelgprs.com","",""},
	{"405","01","rcomnet","",""},
	{"405","03","rcomnet","",""},
	{"405","04","rcomnet","",""},
	{"405","05","rcomnet","",""},
	{"405","06","rcomnet","",""},
	{"405","07","rcomnet","",""},
	{"405","08","rcomnet","",""},
	{"405","09","rcomnet","",""},
	{"405","10","rcomnet","",""},
	{"405","11","rcomnet","",""},
	{"405","12","rcomnet","",""},
	{"405","13","rcomnet","",""},
	{"405","14","rcomnet","",""},
	{"405","15","rcomnet","",""},
	{"405","17","rcomnet","",""},
	{"405","18","rcomnet","",""},
	{"405","19","rcomnet","",""},
	{"405","20","rcomnet","",""},
	{"405","21","rcomnet","",""},
	{"405","22","rcomnet","",""},
	{"405","23","rcomnet","",""},
	{"405","025","TATA.DOCOMO.INTERNET","",""},
	{"405","026","TATA.DOCOMO.INTERNET","",""},
	{"405","027","TATA.DOCOMO.INTERNET","",""},
	{"405","028","TATA.DOCOMO.INTERNET","",""},
	{"405","029","TATA.DOCOMO.INTERNET","",""},
	{"405","030","TATA.DOCOMO.INTERNET","",""},
	{"405","030","TATA.DOCOMO.DIVE.IN","",""},
	{"405","031","TATA.DOCOMO.INTERNET","",""},
	{"405","032","TATA.DOCOMO.INTERNET","",""},
	{"405","033","TATA.DOCOMO.INTERNET","",""},
	{"405","034","TATA.DOCOMO.INTERNET","",""},
	{"405","035","TATA.DOCOMO.INTERNET","",""},
	{"405","036","TATA.DOCOMO.INTERNET","",""},
	{"405","037","TATA.DOCOMO.INTERNET","",""},
	{"405","038","TATA.DOCOMO.INTERNET","",""},
	{"405","039","TATA.DOCOMO.INTERNET","",""},
	{"405","040","TATA.DOCOMO.INTERNET","",""},
	{"405","041","TATA.DOCOMO.INTERNET","",""},
	{"405","042","TATA.DOCOMO.INTERNET","",""},
	{"405","043","TATA.DOCOMO.INTERNET","",""},
	{"405","044","TATA.DOCOMO.INTERNET","",""},
	{"405","44","TATA.DOCOMO.INTERNET","",""},
	{"405","045","TATA.DOCOMO.INTERNET","",""},
	{"405","046","TATA.DOCOMO.INTERNET","",""},
	{"405","047","TATA.DOCOMO.INTERNET","",""},
	{"405","025","Tata.Docomo.internet","",""},
	{"405","51","airtelgprs.com","",""},
	{"405","52","airtelgprs.com","",""},
	{"405","53","airtelgprs.com","",""},
	{"405","54","airtelgprs.com","",""},
	{"405","55","airtelgprs.com","",""},
	{"405","56","airtelgprs.com","",""},
	{"405","66","www","",""},
	{"405","66","portalnmms","",""},
	{"405","67","www","",""},
	{"405","67","portalnmms","",""},
	{"405","70","internet","",""},
	{"405","750","www","",""},
	{"405","750","portalnmms","",""},
	{"405","751","www","",""},
	{"405","751","portalnmms","",""},
	{"405","752","www","",""},
	{"405","752","portalnmms","",""},
	{"405","753","www","",""},
	{"405","753","portalnmms","",""},
	{"405","754","www","",""},
	{"405","754","portalnmms","",""},
	{"405","755","www","",""},
	{"405","755","portalnmms","",""},
	{"405","756","www","",""},
	{"405","756","portalnmms","",""},
	{"405","799","internet","",""},
	{"405","800","aircelgprs","",""},
	{"405","800","aircelwap","",""},
	{"405","801","aircelgprs","",""},
	{"405","802","aircelgprs","",""},
	{"405","803","aircelgprs","",""},
	{"405","804","aircelgprs","",""},
	{"405","805","aircelgprs","",""},
	{"405","806","aircelgprs","",""},
	{"405","807","aircelgprs","",""},
	{"405","808","aircelgprs","",""},
	{"405","809","aircelgprs","",""},
	{"405","810","aircelgprs","",""},
	{"405","811","aircelgprs","",""},
	{"405","812","aircelgprs","",""},
	{"405","823","vinternet.com","",""},
	{"405","824","vinternet.com","",""},
	{"405","825","vinternet.com","",""},
	{"405","826","vinternet.com","",""},
	{"405","827","vinternet.com","",""},
	{"405","828","vinternet.com","",""},
	{"405","829","vinternet.com","",""},
	{"405","830","vinternet.com","",""},
	{"405","831","vinternet.com","",""},
	{"405","832","vinternet.com","",""},
	{"405","833","vinternet.com","",""},
	{"405","834","vinternet.com","",""},
	{"405","835","vinternet.com","",""},
	{"405","836","vinternet.com","",""},
	{"405","837","vinternet.com","",""},
	{"405","838","vinternet.com","",""},
	{"405","839","vinternet.com","",""},
	{"405","840","vinternet.com","",""},
	{"405","841","vinternet.com","",""},
	{"405","842","vinternet.com","",""},
	{"405","843","vinternet.com","",""},
	{"405","845","internet","",""},
	{"405","846","internet","",""},
	{"405","847","internet","",""},
	{"405","848","internet","",""},
	{"405","849","internet","",""},
	{"405","850","internet","",""},
	{"405","851","internet","",""},
	{"405","852","internet","",""},
	{"405","853","internet","",""},
	{"405","881","gprs.stel.in","",""},
	{"405","882","gprs.stel.in","",""},
	{"405","883","gprs.stel.in","",""},
	{"405","884","gprs.stel.in","",""},
	{"405","885","gprs.stel.in","",""},
	{"405","886","gprs.stel.in","",""},
	{"405","908","internet","",""},
	{"405","909","internet","",""},
	{"405","910","internet","",""},
	{"405","911","internet","",""},
	{"405","932","vinternet.com","",""},
	{"426","01","internet.batelco.com","",""},
	{"426","02","internet","internet","internet"},
	{"440","10","mopera.net","",""},
	{"440","10","mpr2.bizho.net","",""},
	{"440","20","open.softbank.ne.jp","opensoftbank","ebMNuX1FIHg9d3DA"},
	{"440","20","smile.world","dna1trop","so2t3k3m2a"},
	{"452","01","m-wap","mms",""},
	{"452","02","m3-world","mms",""},
	{"452","04","v-internet","",""},
	{"452","05","internet","",""},
	{"452","07","internet","mms",""},
	{"454","00","hkcsl","",""},
	{"454","02","hkcsl","",""},
	{"454","03","mobile.three.com.hk","",""},
	{"454","04","web-g.three.com.hk","",""},
	{"454","06","SmarTone-Vodafone","",""},
	{"454","07","3gwap","",""},
	{"454","10","hkcsl","",""},
	{"454","12","peoples.net","",""},
	{"454","16","pccwdata","",""},
	{"454","19","pccw","",""},
	{"455","00","smartgprs","",""},
	{"455","01","ctm-mobile","",""},
	{"455","03","web-g.three.com.hk","hutchison","1234"},
	{"455","04","ctm-mobile","",""},
	{"460","00","cmwap","",""},
	{"460","00","cmnet","",""},
	{"460","01","3gnet","",""},
	{"460","06","UNIM2M.NJM2MAPN","",""},
	{"460","01","uninet","",""},
	{"460","01","3gwap","",""},
	{"460","01","uniwap","",""},
	{"460","02","cmwap","",""},
	{"460","02","cmnet","",""},
	{"460","03","ctwap","ctwap@mycdma.cn","vnet.mobi"},
	{"460","03","ctnet","ctnet@mycdma.cn","vnet.mobi"},
	{"460","07","cmwap","",""},
	{"460","07","cmnet","",""},
	{"460","12","ctwap","ctwap@mycdma.cn","vnet.mobi"},
	{"460","12","ctnet","ctnet@mycdma.cn","vnet.mobi"},
	{"460","13","ctwap","ctwap@mycdma.cn","vnet.mobi"},
	{"460","13","ctnet","ctnet@mycdma.cn","vnet.mobi"},
	{"466","01","internet","",""},
	{"466","88","internet","",""},
	{"466","89","auroraweb","",""},
	{"466","89","vibo","",""},
	{"466","92","internet","",""},
	{"466","92","emome","",""},
	{"466","93","internet","",""},
	{"466","93","twm","",""},
	{"466","97","internet","",""},
	{"466","97","twm","",""},
	{"466","99","internet","",""},
	{"466","99","twm","",""},
	{"470","01","gpinternet","",""},
	{"470","02","internet","",""},
	{"470","03","blweb","",""},
	{"470","04","wap","",""},
	{"470","07","internet","",""},
	{"502","12","net","maxis","wap"},
	{"502","12","unet","maxis","wap"},
	{"502","13","celcom3g","",""},
	{"502","16","diginet","digi","digi"},
	{"502","18","my3g","",""},
	{"502","19","celcom3g","",""},
	{"505","01","Telstra.wap","",""},
	{"505","02","internet","",""},
	{"505","02","yesinternet","",""},
	{"505","03","vfinternet.au","",""},
	{"505","03","purtona.wap","",""},
	{"505","06","3netaccess","",""},
	{"505","06","3services","",""},
	{"505","07","vfinternet.au","",""},
	{"505","11","Telstra.wap","",""},
	{"505","12","3netaccess","",""},
	{"505","12","3services","",""},
	{"505","19","data.lycamobile.com.au","",""},
	{"505","71","Telstra.wap","",""},
	{"505","72","Telstra.wap","",""},
	{"505","88","vfinternet.au","",""},
	{"505","90","internet","",""},
	{"515","02","http.globe.com.ph","",""},
	{"515","03","Smart1","",""},
	{"515","03","internet","",""},
	{"515","05","MINTERNET","",""},
	{"515","18","redmms","",""},
	{"525","01","e-ideas","",""},
	{"525","03","sunsurf","65","user123"},
	{"525","05","internet","",""},
	{"525","05","shwap","star","hub"},
	{"530","01","www.vodafone.net.nz","",""},
	{"530","02","www.vodafone.net.nz","",""},
	{"530","05","internet.telecom.co.nz","",""},
	{"530","24","internet","",""},
	{"602","01","alowap","",""},
	{"602","02","wap.vodafone.com.eg","wap","wap"},
	{"602","03","etisalat","",""},
	{"610","02","internet","internet","internet"},
	{"621","20","internet.ng.zain.com","internet","internet"},
	{"621","20","internet.ng.airtel.com","",""},
	{"621","20","internet.ng.airtel.com","internet","internet"},
	{"621","30","web.gprs.mtnnigeria.net","web","web"},
	{"621","50","gloflat","flat","flat"},
	{"621","50","glosecure","",""},
	{"621","60","etisalat","",""},
	{"639","02","safaricom","saf","data"},
	{"639","03","internet","",""},
	{"639","05","internet","",""},
	{"639","07","orange internet","02web","02web"},
	{"655","01","internet","",""},
	{"655","01","vlive","",""},
	{"655","02","internet","",""},
	{"655","07","Internet","",""},
	{"655","07","vdata","",""},
	{"655","10","internet","",""},
	{"655","10","myMTN","mtnwap",""},
	{"724","02","timbrasil.br","tim","tim"},
	{"724","02","*600","wap@tim","tim"},
	{"724","03","timbrasil.br","tim","tim"},
	{"724","03","*600","wap@tim","tim"},
	{"724","04","timbrasil.br","tim","tim"},
	{"724","04","*600","wap@tim","tim"},
	{"724","05","java.claro.com.br","Claro","claro"},
	{"724","06","zap.vivo.com.br","vivo","vivo"},
	{"724","07","sercomtel.com.br","sercomtel","sercomtel"},
	{"724","10","zap.vivo.com.br","vivo","vivo"},
	{"724","11","zap.vivo.com.br","vivo","vivo"},
	{"724","16","wap.brt.br","brt","brt"},
	{"724","16","wap.brt.br","brt","brt"},
	{"724","19","gprs.telemigcelular.com.br","celular","celular"},
	{"724","23","zap.vivo.com.br","vivo","vivo"},
	{"724","24","gprs.amazoniacelular.com.br","celular","celular"},
	{"724","31","wapgprs.oi.com.br","oiwap","oioioi"},
	{"724","31","*600","oiwap","oioioi"},
	{"730","01","bam.entelpcs.cl","entelpcs","entelpcs"},
	{"730","02","wap.tmovil.cl","wap","wap"},
	{"730","03","bam.clarochile.cl","clarochile","clarochile"},
	{"730","10","bam.entelpcs.cl","entelpcs","entelpcs"},
	{"730","10","imovil.entelpcs.cl","entelpcs","entelpcs"},
	{"520","18","www.dtac.co.th","",""},
	{"520","01","internet","ais","ais"},
	{"520","99","internet","true","true"},
	{"520","15","internet","",""},
	{"520","00","hinternet","true","true"},
	{"510","10","telkomsel","wap","wap123"},
	{"510","01","indosatgprs","indosat","indosat"},
	{"510","11","www.xlgprs.net","xlgprs","proxl"},
	{"510","89","3gprs","3gprs","3gprs"},
	{"510","08","AXIS","axis","123456"},
	{"744","01","vox.internet","",""},
	{"744","02","internet.ctimovil.com.py","",""},
	{"744","04","internet.tigo.py","",""},
	{"744","05","internet","",""},
	{"620","01","internet","",""},
	{"620","02","browse","",""},
	{"620","03","web.tigo.com.gh","",""},
	{"620","06","web","",""},
	{"620","07","internet","",""}
};
const u16 apn_num = (sizeof(apn_table) / sizeof(apns));
