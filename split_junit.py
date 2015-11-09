import re
from lxml import etree
import xml.etree.ElementTree as ET
import json

end_delimiter = "</testsuite>"

if __name__ == '__main__':

    # open JUnit result output file sent via COM port
    with open ("output.xml", "r") as myfile:
        data = myfile.read()

    # Split xml to separate testsuite results
    s =  [e+end_delimiter for e in data.split(end_delimiter) if e != ""]
    
    for next in s:
        
        next = next.strip('\n')
        next = re.sub('^.*<','<', next)

        # TODO: Handle singleton failure element
        
        if next != end_delimiter:
                        
            # Remove properties element -- it makes etree explode
            #next = re.sub('<properties>.*</properties>','', next)
            
            print 
            print next
            print
            
            # Parse the XML
            root = ET.fromstring(next)
            
#           print json.dumps(root.attrib, indent=1)
                        
            testsuite_name = root.get('name')

            tree = ET.ElementTree(root)
            tree.write(testsuite_name+".xml", encoding='utf-8', xml_declaration=True)
        