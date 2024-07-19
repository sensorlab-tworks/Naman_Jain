//Library
#include <WiFi.h>
#include <WebServerSecure.h>
#include <FS.h>
#include <LEAmDNS.h>

// network credentials and variables
const char* ssid = "VNSMonitor";
const char* password = "vagaltone";
// variables

struct user_info temp_user_data;
const 
String ID1;
// SSL certificate and private key (use actual certificate and key content)
static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDezCCAmMCFBJEWebNmGNFh3KQfjPDdaRQ0GkTMA0GCSqGSIb3DQEBCwUAMHox
CzAJBgNVBAYTAklOMRIwEAYDVQQIDAlIeWRlcmFiYWQxEjAQBgNVBAcMCUN5YmVy
YWJhZDEMMAoGA1UECgwDVk5TMQwwCgYDVQQLDANWTlMxEzARBgNVBAMMClZOU01v
bml0b3IxEjAQBgkqhkiG9w0BCQEWAyBuYTAeFw0yNDA2MjQwNjA4MDBaFw0yNTA2
MjQwNjA4MDBaMHoxCzAJBgNVBAYTAklOMRIwEAYDVQQIDAlIeWRlcmFiYWQxEjAQ
BgNVBAcMCUN5YmVyYWJhZDEMMAoGA1UECgwDVk5TMQwwCgYDVQQLDANWTlMxEzAR
BgNVBAMMClZOU01vbml0b3IxEjAQBgkqhkiG9w0BCQEWAyBuYTCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAKXv7YYhCgTWJql16nojm0z+cebHnzGRvtHV
nZC+9pDy/+vE5Pvf+5B80ujX+OuE8iwnA8JG6MnSNmnjjwNIoJ9vNOJkN57IhP07
lLOPVA6tiSd0mJFyP49efpg/+tAJB+/bqbSzs+JljeadZCQ9YzCNolj0l559JYnu
kOqisdI5jHJj87Tn/ilkrjGsb8d2rTELbDKr8cvZbxiyPQnYDiIF7o3vbVhaoix1
0vlEoAlr4FBSrtzbzemPKlvF2Dkx6odZtEqC3B+LRCkL2GmeIEp/F7pxAiMKV5o2
76F0CTLWXsIHCCXFVxbAgz0j5SCMZdmIUQnTdUsw+SwqOMJl4EkCAwEAATANBgkq
hkiG9w0BAQsFAAOCAQEAL5/HkVwnMQcAC/u2+WntJAfVZE+8Ve8F03U7BB8m+kwv
t4mF0rPeszL3cxExeCZI/N4vpUt87fEdQDd+0fjKRsXrYzdZ5XK+dLbfX03nqlI1
/FGRghT739Cn+/PH1VzndIfZBAuGOxMGHH0C7S1tDPTexV5wOIDY2hPLHLRxLzFn
rGxPvc14EGZ7tKYxq8uTWvW94yj3IZ8rmADiMIg1YxwpdWnkQh6xgvPcHgceqpKR
YNyqYkSdDLa8CbAFk30JkX9NICA+mPjwoM3Ie77gsj+qDF9yjJZu0bF29ohzziTs
p+OTV5hgOF0+EDjW/WMQipWAZU1VikS0lB/phyhT8g==
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCl7+2GIQoE1iap
dep6I5tM/nHmx58xkb7R1Z2QvvaQ8v/rxOT73/uQfNLo1/jrhPIsJwPCRujJ0jZp
448DSKCfbzTiZDeeyIT9O5Szj1QOrYkndJiRcj+PXn6YP/rQCQfv26m0s7PiZY3m
nWQkPWMwjaJY9JeefSWJ7pDqorHSOYxyY/O05/4pZK4xrG/Hdq0xC2wyq/HL2W8Y
sj0J2A4iBe6N721YWqIsddL5RKAJa+BQUq7c283pjypbxdg5MeqHWbRKgtwfi0Qp
C9hpniBKfxe6cQIjCleaNu+hdAky1l7CBwglxVcWwIM9I+UgjGXZiFEJ03VLMPks
KjjCZeBJAgMBAAECggEABeKJsbdTv+Kpadqi93RyUkt21iivumYRFqeFCEhhfbgE
gAPSXCVwKkpm/3eDXjTvksXKchYqNNgl8mDt5j+p5KWgwkOsHk89ui5mAm07na6l
XdcafxQGcYr4IeWo+z5KwASyEtTJtSaFXw+naMqdjTAEuTNpeic1kNv7WzrsZAh0
UuH3Qt2KaKKRaZg9674IwC5LOXZ9y/CjdaN4/0c039xQlQINh+E32egJGxpA2DiH
L9m3pUngQI8NnddQznE2nm9fyg4IioEbqp2Qwyue3/5feEO2npRogY5c8diA8W5I
WtWS67KobQEikJmE2P7GU2tAyQBo9NRHTb7gZw6v4QKBgQDFXGB1NH359JUUV9VH
N6oiDK0HHFCjgR3EJQ5gbQIq/dVPv3Dnh+hmeGGCLyB/xlR1qMRVVWWEkF9HfYls
mZIPgwXRmmT9kvhB0hNi7k6+ViIrB06HTmUr+LzsbXaNfE92rKBL7U6LO60df9LX
mjDeFm1i+C/O05GXTzwWSU41kQKBgQDXPWm39bX1GOt2Qb5RyNScvcP8F7y579Ev
LOwEnzx3/gg0Q0IwBS1lWfCSo9OsHnuhIjX85e9USYpnjAyaIW0EjVGhYYWH6t4p
uAIxRvMwGit5cosLvyL/2lMo0kyIvqAK9WKTqHF+7jTYgqDwnFhbGvaPbY9HRv7/
1UFi8QdDOQKBgAJXyGVbzCsNdj+Zk1SXGpPw5pTxBxwJylqrvfHzGVV9Uezybtqw
yFd/EllDSfV6W0JSUJJawyvBsv/cQ6dgY1V/zq/gT1abSgst/UDtZ03nI9yy54G/
sn/FIikEW6XuVdsozXZZSNdqS/HPtKcOk4nvZjkIjTi9Vjsfm60ADcehAoGBAMJF
8zMQu3CDUpwFkRPeklD7U3cpsbWeCRBsmoc9duXTS3JotsgDFEtjl6mz5y12wh3Z
dCNVIuTQD/WQShugSfcO2vwvusfILUg4hvoVURE8vbq7sVcj6SJH3EF/N4dBj9D0
VUpzgiWQOfCmTz99zp5EwwEIYYdzRFyEDqWQaf65AoGAY13EMjm63i0EpE8P0hhx
xekB5WICVxLQ6Uoxl+I50pvVNRXsrQdrNeIUwtLRdYMDUFj0Ha3a/PARr1FS2Qyz
HCsCXOZx/p7SoYRK2BrAFwqTba3XCl7YNXcYqYLSWCSdObmyZyTBMN9/fw47VGb3
0hHfmjtYsfpfi09jBEXDK5Q=
-----END PRIVATE KEY-----
)EOF";

//Create a secure web server on port 443 (default HTTPS port)

WebServerSecure server(443);
ServerSessions serverCache(5);
// fuctions
struct user_info getID()
 {
  while(1)
   {
 // Handle client requests
    server.handleClient();
     if (temp_user_data.ID!="")
     {
      break;
     }
  }
  return temp_user_data;
 }
void handleForm() {
  // HTML form to be sent to the client
  const char formHTML[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
        body {
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          height: 100vh;
          margin: 0;
          font-family: Arial, sans-serif;
          background-color: #00B3B2;
          color: #FFFFFF; /* Text color */
        }
        .container {
          width: 100%;
          text-align: center; /* Center-align content */
          display: flex;
          flex-direction: column;
          align-items: center;
          margin-top: 5%; /* Increased top margin */
          margin-bottom: 5%; /* Margin at the bottom */
        }
        .inputBox {
          height: 20%; /* Reduced height of inputBox1 */
          width: 90%; /* Width for inputBox1 */
          margin-bottom: 5%;
          padding: 1%;
          box-sizing: border-box;
          text-align: center; /* Center-align text */
        }
        .submitButton {
          height: 20%; /* Adjusted height to match inputBox */
          width: 60%; /* Adjusted width to match inputBox */
          margin-bottom: 5%;
          padding: 1%;
          box-sizing: border-box;
          text-align: center; /* Center-align text */
          background-color: #005D5D; /* Button background color */
          color: #FFFFFF; /* Button text color */
          border: none; /* Remove border */
          cursor: pointer; /* Pointer cursor on hover */
          border-radius: 8px; /* Rounded corners */
        }
        .submitButton:hover {
          background-color: #0056b3; /* Darker blue on hover */
        }

        h1 {
          margin-bottom: 20px; /* Adjust margin for spacing */
        }
      </style>
      <title>Vagal Monitoring System</title>
    </head>
    <body>
      <h1>Vagal Monitoring System</h1>
      <div class="container">
        <input type="text" id="ID" class="inputBox" placeholder="Enter ID here">
        <input type="text" id="Name" class="inputBox" placeholder="Enter Name here">
        <input type="text" id="Date_time" class="inputBox" readonly>
        <input type="text" id="Duration" class="inputBox" placeholder="Enter Duration here">
        <input type="text" id="Age" class="inputBox" placeholder="Enter Age here">
        <input type="text" id="Gender" class="inputBox" placeholder="Enter Gender here">
        <input type="text" id="Weight" class="inputBox" placeholder="Enter weight here">
        <button id="submitButton" class="submitButton">Submit</button>
      </div>
      <script>
        // Function to get URL query parameter
        function getQueryParam(param) {
          var urlParams = new URLSearchParams(window.location.search);
          return urlParams.get(param);
        }

        // Set the value of ID if the query parameter exists
        var inputValueFromURL1 = getQueryParam('ID');
        if (inputValueFromURL1) {
          document.getElementById('ID').value = decodeURIComponent(inputValueFromURL1);
        }
        // Set the value of Name if the query parameter exists
        var inputValueFromURL2 = getQueryParam('Name');
        if (inputValueFromURL2) {
          document.getElementById('Name').value = decodeURIComponent(inputValueFromURL2);
        }
        // Set the value of duration if the query parameter exists
        var inputValueFromURL4 = getQueryParam('Duration');
        if (inputValueFromURL4) {
          document.getElementById('Duration').value = decodeURIComponent(inputValueFromURL4);
        }
        else
        {
          document.getElementById('Duration').value=300;
          }
        // Set the value of Age if the query parameter exists
        var inputValueFromURL5 = getQueryParam('Age');
        if (inputValueFromURL5) {
          document.getElementById('Age').value = decodeURIComponent(inputValueFromURL5);
        }// Set the value of Gender if the query parameter exists
        var inputValueFromURL6 = getQueryParam('Gender');
        if (inputValueFromURL6) {
          document.getElementById('Gender').value = decodeURIComponent(inputValueFromURL6);
        }// Set the value of Weight if the query parameter exists
        var inputValueFromURL7 = getQueryParam('Weight');
        if (inputValueFromURL7) {
          document.getElementById('Weight').value = decodeURIComponent(inputValueFromURL7);
        }

        // Set the current time and date in inputBox3
        function setCurrentDateTime() {
          var now = new Date();
          var year = now.getFullYear();
          var month = ('0' + (now.getMonth() + 1)).slice(-2);
          var day = ('0' + now.getDate()).slice(-2);
          var sec = ('0' + now.getSeconds()).slice(-2);
          var mins = ('0' + now.getMinutes()).slice(-2);
          var hrs = ('0' + now.getHours()).slice(-2);
          var formattedDateTime = year + '-' + month + '-' + day + '-' + '-' + hrs +'-' + mins +'-'+ sec;
          document.getElementById('Date_time').value = formattedDateTime;
        }
        setCurrentDateTime();
        setInterval(setCurrentDateTime, 1000); // Update the time every second

        // Handle the button click event
        document.getElementById('submitButton').addEventListener('click', function() {
          var ID = document.getElementById('ID').value;
          var Name = document.getElementById('Name').value;
          var Date_time = document.getElementById('Date_time').value;
          var Duration = document.getElementById('Duration').value;
          var Age = document.getElementById('Age').value;
          var Gender = document.getElementById('Gender').value;
          var Weight = document.getElementById('Weight').value;

          // Create an object to hold the values and send it to the server
          var dataToSend = {
           ID: ID,
            Name: Name,
            Date_time:Date_time,
            Duration: Duration,
            Age : Age,
            Weight:Weight,
            Gender:Gender
            
          };

          // Convert the object to a query string
          var queryString = Object.keys(dataToSend).map(function(key) {
            return key + '=' + encodeURIComponent(dataToSend[key]);
          }).join('&');

          // Redirect to the /get endpoint with the concatenated text
          window.location.href = '/get?' + queryString;
           
        });
      </script>
    </body>
    </html>
  )rawliteral";

  // Send the HTML form to the client
  server.send(200, "text/html", formHTML);
}

void handleGet() {
  String ID = server.arg("ID");
  String Name = server.arg("Name");
  String Date_time = server.arg("Date_time");
  String Duration = server.arg("Duration");
  String Age = server.arg("Age");
  String Gender = server.arg("Gender");
  String Weight = server.arg("Weight");
  temp_user_data.ID=ID;
  temp_user_data.Name=Name;
  temp_user_data.Date_time=Date_time;
  temp_user_data.Duration= Duration.toInt();
  temp_user_data.Age= Age;
  temp_user_data.Gender= Gender;
  temp_user_data.Weight= Weight;
  Serial.println("ID: " + ID);
  Serial.println("Name: " +Name);
  Serial.println("Date_time: " + Date_time);
  Serial.println("Duration: " + Duration);
  Serial.println("Age: " + Age);
  Serial.println("Gender: " + Gender);
  Serial.println("Weight: " + Weight);

  String response = "<!DOCTYPE html><html><body><h1>You entered:</h1>";
  response += "<p>ID: " + ID + "</p>";
  response += "<p>Name: " + Name + "</p>";
  response += "<p>Current Time: " + Date_time + "</p>";
  response += "<p>Duration: " + Duration + "</p>";
  response += "<p>Age: " + Age + "</p>";
  response += "<p>Gender: " + Gender + "</p>";
  response += "<p>Weight: " + Weight + "</p>";
  response += "</body></html>";

  
  server.send(200, "text/html", response);
}

bool startAP()
{
  delay(2000);//debug
  Serial.begin(115200);
  Serial.println("Setting up AP...");
  if (WiFi.softAP(ssid, password)) {
    Serial.println("Access Point started successfully.");
  } else {
    Serial.println("Failed to start Access Point.");
    return false;
  }
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  delay(2000);
  Serial.println("IP time?");
  // Give some time for the AP to be set up
  delay(1000);
  Serial.println("IP time");
   
  // Print the IP address
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  if (MDNS.begin("picow")) { Serial.println("MDNS responder started"); }
  
  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  
  // Cache SSL sessions to accelerate the TLS handshake.
  server.getServer().setCache(&serverCache);
  
  // Define request handlers
  server.on("/", handleForm);
  server.on("/get", handleGet);
  
  // Start the server
  server.begin();
  Serial.println("HTTPS server started");
  
  // make sure temp_user_data is empty
  memset(&temp_user_data, 0, sizeof(temp_user_data));
  
  return true;
  
}

void stopAP(){
  WiFi.softAPdisconnect(true);
  Serial.println("SoftAP Stopped");
}
