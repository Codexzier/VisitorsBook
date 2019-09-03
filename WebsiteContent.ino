// ========================================================================================
// Description:       Used connect to wlan to get information on webbrowser
// ========================================================================================

void BuildWebside(WiFiClient webclient){
  
  webclient.println("HTTP/1.1 200 OK");
  webclient.println("Content-Type: text/html");
  webclient.println("");                           // one row must be empty
  webclient.println("<!DOCTYPE HTML>");
  webclient.println("<html>");

  PrintHtmlHead(webclient);
  PrintBodyContent(webclient);

  webclient.println("</html>");
}

void PrintHtmlHead(WiFiClient webclient) {

  webclient.println("<head>");
  
  webclient.println("<title>");
  webclient.println("Visitors book");
  webclient.println("</title>");
  
  webclient.println("<style>");
  
  webclient.println("h1 { color:green; font-size:5rem; }");
  webclient.println(".divTable{ display: table;width: 50rem;}");
  webclient.println(".divTableRow {display: table-row;}");
  webclient.println(".divTableHeading { background-color: #EEE;display: table-header-group; }");
  webclient.println(".divTableCell, .divTableHead { width: 30rem; border: 1px solid #999999;display: table-cell;padding: 3px 10px;font-size: 3rem;}");
  webclient.println(".divTableCellLeft {width: 150px;border: 1px solid #999999;display: table-cell;padding: 3px 10px; }");
  webclient.println(".divTableHeading {background-color: #EEE; display: table-header-group;font-weight: bold; }");
  webclient.println(".divTableFoot {background-color: #EEE;display: table-footer-group;font-weight: bold; }");
  webclient.println(".divTableBody {display: table-row-group; }");

  webclient.println(".divTableRowDiagram {display: table-row; height: 100px; width: 320px;}");
  webclient.println(".divTableCellDiagram {width: 1px;display: table-cell;padding: 0px 0px;background-color: rgb(207, 207, 207); vertical-align: bottom; }");
  webclient.println(".divTableCellDiagramValue {background-color: rgb(35, 116, 25); }");
  webclient.println("input { font-size: 3rem; }");
  webclient.println("button { font-size: 3rem; }");
  
  webclient.println("</style>");
  
  webclient.println("</head>");
}

void PrintBodyContent(WiFiClient webclient) {
  webclient.println("<body>");
  webclient.println("");

  webclient.println("<h1>Visitors book</h1>");

  if(mShowErrorMessage){
    PrintError(webclient);
  }
  
  PrintGuestInput(webclient);
  webclient.println("<div style=\"height: 20px\"></div>");
  PrintVisitors(webclient);
  
  webclient.println("</body>");
}

void PrintGuestInput(WiFiClient webclient){
  webclient.println("<div class=\"divTable\" style=\"border: 1px solid rgb(95, 95, 95);\" >");
  webclient.println("<div class=\"divTableBody\">");
  webclient.println("<div class=\"divTableRow\">");
  webclient.println("<div class=\"divTableCell\">Visitor</div>");
  webclient.println("</div>");
  
  webclient.println("<form id=\"saveGuest\" action=\"192.168.4.1\">");
  webclient.println(" <div class=\"divTableRow\">");
  webclient.println("  <div class=\"divTableCell\">Name:</div>");
  webclient.println("  <div class=\"divTableCell\"><input type=\"text\" name=\"guestname\" /></div>");
  webclient.println(" </div>");
  webclient.println(" <div class=\"divTableRow\">");
  webclient.println("  <div class=\"divTableCell\">actual time:</div>");
  webclient.println("  <div class=\"divTableCell\">");

  webclient.print("<input type=\"datetime-local\" name=\"settime\" value=\"\" min=\"2019-06-01T08:30\" >");
  
  webclient.println(" </div>");
  webclient.println(" </div>");
  webclient.println(" <div class=\"divTableRow\">");
  webclient.println("  <div class=\"divTableCell\"></div>");
  webclient.println("  <div class=\"divTableCell\"><button id=\"saveGuest\" type=\"submit\">Save</button></div>");
  webclient.println(" </div>");
  webclient.println("</form>");
  
  webclient.println("</div>");
  webclient.println("</div>");
}

void PrintVisitors(WiFiClient webclient){
  webclient.println("<div class=\"divTable\" style=\"border: 1px solid rgb(95, 95, 95);\" >");
  webclient.println(" <div class=\"divTableBody\">");

  ReadVisitorsAndPrintToWeb(webclient);
  
  webclient.println(" </div>");
  webclient.println("</div>");
}

void PrintError(WiFiClient webclient){
  webclient.println("<div class=\"divTable\" style=\"border: 1px solid rgb(255, 0, 0);\" >");
  webclient.println(" <div class=\"divTableBody\">");

  webclient.println("  <div class=\"divTableRow\">");
  webclient.println("   <div class=\"divTableCell\">Error:</div>");
  webclient.println("   <div class=\"divTableCell\" style=\"color:red\">");
  webclient.println(mErrorMessage);
  webclient.println("   </div>");
  webclient.println("  </div>");
  
  webclient.println(" </div>");
  webclient.println("</div>");
}
