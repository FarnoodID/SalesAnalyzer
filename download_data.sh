#!/bin/bash
mkdir -p /tmp/Sales_Analyzer
wget -O /tmp/Sales_Analyzer/report.tar 'http://donwloadthefile.com/report.gz.tar'
tar -xzvf /tmp/Sales_Analyzer/report.tar -C /tmp/Sales_Analyzer
rm /tmp/Sales_Analyzer/report.tar
