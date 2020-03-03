'use strict'

// C library API
const ffi = require('ffi');


// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 
const mysql = require('mysql');
let connection;
/**
 * FFI crap
 */
let sharedLib = ffi.Library('./sharedLib.so', {
  //'FunctionName':['return value(int, string), ['function params],[function param]],FunctionName':['return value(int, string), ['function params],[function param]]
  'serverCreateCalendar':['string',['string']], 'serverUserCal':['string', ['string', 'string', 'string', 'string', 'string', 'string', 'string']],
  'serverEventList':['string',['string']],
  'serverAddEvt':['string',['string', 'string', 'string', 'string', 'string']],
  'serverFindEvent':['string',['string', 'string']],
  'findAlarmList':['string', ['string', 'string']],
  'validateFiles':['string',['string']]

});

/**
 * Server side
 * Gets the list of optional properties inside the desired event
 */
app.get('/getPropList', function(req, res) {
    let fileName = req.query.cal;
    let eventNum = req.query.event;
    let propList = sharedLib.serverFindEvent(fileName, eventNum);
    res.send(propList);
})


/**
 * Server side
 * Gets the list of alarms inside the desired event
 */
app.get('/getAlarmList', function (req, res) {
  let fileName = req.query.cal;
  let eventNum = req.query.event;
  let alarmList = sharedLib.findAlarmList(fileName, eventNum);
res.send(alarmList);
});

/**
 * Server side
 * Create a new calendar based on the users sent data
 */
app.get('/sendCalInfo', function(req, res) {
  let fileNames = req.query.fileName;
  let versionNum = req.query.version;
  let productID = req.query.prodID;
  let calStartDate = req.query.calSDT;
  let calStartTime = req.query.calSTT;
  let calSummary = req.query.calSum;
  let evtUID = req.query.evtUID;
  var aCal = sharedLib.serverUserCal(fileNames, versionNum, productID, evtUID,calStartDate, calStartTime, calSummary);
  res.send(aCal);
})

/**
 * Server side
 * Get the list of events for a specified calendar
 */
app.get('/getEvtList', function(req, res) {
  let fileName = req.query.value;
  var theList = sharedLib.serverEventList(fileName);
  res.send(theList);
});

/**
 * Server side
 * Add an event to a specified calendar
 */
app.get('/addEvt', function(req, res) {
  let fileName = req.query.fileName;
  let uid = req.query.theUid;
  let startDT = req.query.startDate;
  let startT = req.query.startTime;
  let sum = req.query.summary;

  var val = sharedLib.serverAddEvt(fileName, uid, startDT, startT, sum);
  res.send(val);
});

/**
 * Server side
 * Check if the uploads directory is empty
 */
app.get('/uploads', function(req,res) {
  var fs = require('fs');
  const uploads = "uploads"
  fs.readdir(uploads, function(err, files) {
    
    //iterate over the file list checking for invalid cals
    for(var i = 0; i < files.length; i++) {
      if(files[i].includes(".ics")) {
        var fileName = "uploads/" + files[i];
        var check = sharedLib.validateFiles(fileName);
        if(check != "OK")  {
          files[i] = "BAD"
        }
      } else {
        files[i] = "BAD";
      }
    }
    res.send(files);
  });
})

/**
 * Turn the files in the server into JSon strings
 */
app.get('/fileToJSon', function(req, res) {
  var fs = require('fs');
  const upload = "uploads";
  fs.readdir(upload, function(err, files) {
    //iterate over the file list checking for invalid cals by file name
    for(var i = 0; i < files.length; i++) {
      if(files[i].includes(".ics")) {
        var fileName = "uploads/" + files[i];
        var check = sharedLib.validateFiles(fileName);
        if(check != "OK")  {
          files[i] = "BAD"
        }
      } else {
        files[i] = "BAD";
      }
    }
    //check for valid calendar files with validateCal
    var theFiles = [];
    var upload = "uploads/";
      for(var i = 1; i < files.length; i++) {
        if(files[i].includes(".ics")) {
          var path = upload + files[i];
          var check = sharedLib.validateFiles(path);
          if(check == "OK") {
            var aFile = sharedLib.serverCreateCalendar(path);
            theFiles.push(aFile);
          }
          
        }
        
      }
    res.send(theFiles);
  });
});


/**
 * Server side
 * Login to the database with your desired information
 * DONE -- Double check beofre submission
 */
app.get('/dbLogin', function(req, res) {
  let userN = req.query.userName;
  let passW = req.query.password;
  let dbID = req.query.databaseID;

  connection = mysql.createConnection({
    host : 'dursley.socs.uoguelph.ca',
    user : userN,
    password : passW,
    database : dbID
  });

  var fileCheck = "SELECT * FROM FILE";
  var eventCheck = "SELECT * FROM EVENT";
  var alarmCheck = "SELECT * FROM ALARM";
  var makeFile = "CREATE TABLE FILE (cal_id INT AUTO_INCREMENT, file_Name VARCHAR(60) NOT NULL, version INT NOT NULL, prod_id VARCHAR(256) NOT NULL, PRIMARY KEY(cal_id))";
  var makeEvent = "CREATE TABLE EVENT (event_id INT AUTO_INCREMENT, summary VARCHAR(1024), start_time DATETIME NOT NULL, location VARCHAR(60), organizer VARCHAR(256), cal_file INT NOT NULL, PRIMARY KEY(event_id), FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE)";
  var makeAlarm = "CREATE TABLE ALARM (alarm_id INT AUTO_INCREMENT, action VARCHAR(256) NOT NULL, `trigger` VARCHAR(256) NOT NULL, event INT NOT NULL, PRIMARY KEY(alarm_id), FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE)"
  connection.connect( function(err, result) {
    if(err){
      console.log(err);
      res.send("Error on Login");
    } else {
      connection.query(fileCheck, function(err, result) {
        if(err) { //if the file table does not exist make all the tables
            connection.query(makeFile, function(err, result) { //make the file table
              if(err) {
                throw err; //error making fild table
              } else {
                console.log("Made file table")
                connection.query(makeEvent, function(err, result) { //make the event table
                  if(err) {
                    throw err; //error making event table
                  } else {
                    console.log("Made the event table");
                    connection.query(makeAlarm, function(err, result) { //make the alarm table
                      if(err) {
                        throw err; //error makeing alarm table
                      } else {
                        console.log("made the alarm table");
                        res.send("OK");
                      }
                    });
                  }
                });
              }
            });
          console.log("File table is not real");
        } else { //since file table exists, check if the other tables exists
            connection.query(eventCheck, function(err, result) {
              if (err) {
                console.log(err);
                console.log("event table not real");
              } else {
                connection.query(alarmCheck, function(err, result) {
                  if(err) {
                    console.log("alarm table does not exists");
                  } else {
                    res.send(userN);
                  }
                });
              }
            });
          }
      });
    }
  });
  
});

/**
 * Server Side A4
 * Get distinct alarms based on the trigger value
 */
app.get('/getFistinctAlarm', function(req, res) {
  var query = "SELECT DISTINCT `trigger` FROM ALARM";

  connection.query(query, function(err, rows, fields) {
    if(err) {
      console.log(err);
      res.send("Error");
    } else {
      res.send(rows);
    }
    
  });
});

/**
 * Send and alaram query
 */
function alarmQuery(fileN, evtNum, eventID) {
  var alarms = sharedLib.findAlarmList(fileN, evtNum);
              var alarmList = JSON.parse(alarms);
  
            //loop over the list of JSON alarms, getting the required info
            
              for(var a = 0; a < alarmList.length; a++) {
                var trig;
                var act;
  
                trig = alarmList[a].trigger;
                act = alarmList[a].action;
  
  
                var alarmInsert = "INSERT INTO ALARM (action, `trigger`, event) VALUES('" + act + "','" + trig + "'," + eventID +");";
                  connection.query(alarmInsert, function(err, result) {
                    if (err) {
                      return("Error");
                    } 
                
                  });
                }
}

/**
 * Function to send the queries for storing the files in the filelog panel
 */
function sendQuery(fileN, query, index) {
  var file = "uploads/" + fileN;
  connection.query(query, function(err, result) {
   
    if(err) {
      console.log(err);
      return("Error inserting calendar");
    } else {
      
      var evts = sharedLib.serverEventList(file); //get the JSON rep of the event list for the calendar
  
      var evtObj, sum, startTime, organ, locat; //variables for the required event items
      evtObj = JSON.parse(evts); //parse each eventJSON from the eventList
  
      //Loop over every event inside the current calendar and get its required values 
      for(var g = 0; g < evtObj.length; g++) {
        var foundOrg = false;
        var foundLoc = false;
        var iterator = 0;
        var temp = evtObj[g];
        
        var st = temp.startDT.date.slice(0,4) + "-" + temp.startDT.date.slice(4,6) + "-" + temp.startDT.date.slice(6,8) + "T" + temp.startDT.time.slice(0,2) + ":" + temp.startDT.time.slice(2,4) + ":" + temp.startDT.time.slice(4,7) + "Z";
        sum = temp.summary;
        startTime =st;
  
        var charG = "" + (g+1); //turn the int g into a string
        //for every event in the list, get the properties
        var evtProps = sharedLib.serverFindEvent(file, charG);
        var propList = JSON.parse(evtProps);
  
  
        //loop to check for the organizer and location properties
        while(iterator != propList.length) {
          temp = propList[iterator].propName;
  
          if(temp == "ORGANIZER") {
            foundOrg = true;
            var organ = propList[iterator].propDescr;
          } else if (temp == "LOCATION") {
            foundLoc = true;
            var locat = propList[iterator].propDescr;
          } else { //if i have not found the organizer or location prop set their values to NULL
            if(foundOrg == false) {
              organ = null;
            }
            if (foundLoc == false) {
              locat = null;
            }
  
          }
          iterator++;
        }
  
        var evtInsert = "INSERT INTO EVENT (summary, start_time, location, organizer, cal_file) VALUES('" + sum + "','" + startTime + "','" + locat + "','" + organ + "'," + index + ");"; //SQL command to insert a new event
        //ADD THE EVENT SQL QUERY HERER 
        connection.query(evtInsert, function(err, result) {
          if(err) { //if an error occurs in inserting the evt quit
            console.log("Error in event insert");
            return("Error");
          } else { //if the event has been inserted do the events alarms
            var getEvt = "SELECT * FROM EVENT ORDER BY event_id DESC LIMIT 1;"
            connection.query(getEvt, function(err, rows, fields) {
              var evtN = (rows[0].event_id);
                alarmQuery(file, charG ,evtN);
              });
            
            }
          });     
        }
        }
      });  
}

function clearDB() {
  var clearAlarm = "DELETE FROM ALARM;";
  var resetAlarm = "ALTER TABLE ALARM AUTO_INCREMENT=1;"
  var clearEvent = "DELETE FROM EVENT;";
  var resetEvent = "ALTER TABLE EVENT AUTO_INCREMENT=1";
  var clearFile = "DELETE FROM FILE;";
  var resetFile = "ALTER TABLE FILE AUTO_INCREMENT=1";

  connection.query(clearAlarm, function(err, result) { //attempt to clear the rows in the ALARM table
    if(err) {
      console.log("Clear alarm error" + err);
      return("Error");
    } else {
      connection.query(resetAlarm, function(err, result) {
        if(err) {
          console.log("Reset alarm error " + err);
          return("Error");
        } else {
          connection.query(clearEvent, function(err, result) { //attempt to clear the rows in the EVENT table
            if(err) {
              console.log("Clear event error" + err);
              return("Error");
            } else {
              connection.query(resetEvent, function(err, result){
                if (err) {
                  console.log("Reset event error " + err);
                  return("Error");
                } else {
                  connection.query(clearFile, function(err, result) { //attempt to clear the rows in the FILE table
                    if (err) {
                      console.log("Clear file Error" + err);
                      return("Error");
                    } else {
                      connection.query(resetFile, function(err, result) {
                        if(err) {
                          console.log("Reset File Error " + err);
                          return("Error");
                        } else {
                          console.log("Successfully cleared and reset the tables");
                          return("Success");
                        }
                      });
                    }
                  });
                }
              });
            }
          });
        }
      });
    }
  });
}

/**
 * Server Side
 * Create the calendar corresponding to the files in the fileList
 * compare the file names to those already in the TABLE == if there skip them
 * add the files that are left in the array to the table (build with C code, parse the JSON for the info)
 * TEST WITH MULTIPLE CALS
 */
app.get('/dbStore', function(req, res) {
  var files = req.query.fileNames;

    for (var i = 0; i < files.length; i++) {
      var file  = "uploads/" + files[i];
      var fileN = files[i];
      var cals = sharedLib.serverCreateCalendar(file);

      var obj, version, prodID, evtNum;
      var obj = JSON.parse(cals);

      version = obj.version;
      prodID = obj.prodID;
      evtNum = obj.numEvents;
      var calSQL = "INSERT INTO FILE (file_Name, version, prod_id) VALUES ('" + fileN + "'," + version + ",'" + prodID + "');";
      var result = sendQuery(fileN, calSQL, i+1);
      
  }

  
  res.send("Success");
});

/**
 * Server Side
 * Get the number of rows in all the tables
 */
app.get('/displayDB', function(req, res) {
  var cals, evts, alms;
  /*Queries to get the num of rows in each table */
  var numCal = "SELECT COUNT(*) AS calCount FROM FILE";
  var numEvt = "SELECT COUNT(*) AS evtCount FROM EVENT";
  var numAlm = "SELECT COUNT(*) AS almCount FROM ALARM";

  connection.query(numCal, function(err, rows, fields) {
    if(err) {
      console.log("Error getting number of entries in the FILE table");
      console.log(err);
      res.send("Error getting f");
    } else {
      cals = rows[0].calCount;

      connection.query(numEvt, function(err, rows, fields) {
        if(err) {
          console.log("Error getting number of entries in EVENT table");
          res.send("Error");
        } else {
          evts = rows[0].evtCount;
          connection.query(numAlm, function(err, rows, fields) {
            if(err) {
              console.log("Error getting number of entries in ALARM table");
              res.send("Error");
            } else {
               alms = rows[0].almCount; 
               res.send({
                cals,
                evts,
                alms
              }); 
            }
          });
        }
      });
    }
  });
});

/**
 * Server Side A4
 * Display all events in the DB
 */
app.get('/displayEvts', function(req, res) {
  var evtQuery = "SELECT * FROM EVENT ORDER BY start_time";

  connection.query(evtQuery, function(err, rows, fields) {
    res.send(rows);
  });
});

/**
 * Server Side A4
 * Display all events within a calendar
 */
app.get('/getCalEvt', function(req, res) {
  var fileN = req.query.file;//get the file name

  var calQ = "SELECT cal_id FROM FILE WHERE file_Name = '" + fileN + "';";
  connection.query(calQ, function(err, rows, fields) {
    if(err) {
      console.log("Cal error : " + err);
      res.send(err);
    } else {
      var cal = rows[0].cal_id;

      var evtQ = "SELECT * FROM EVENT WHERE cal_file = " + cal + ";";
       connection.query(evtQ, function(err, rows, fields) {
         if(err) {
           console.log("Evt error: " + err);
           res.send(err);
         } else {
           console.log(rows);
           res.send(rows);
         }
       });
    };  
  });
});

/**
 * Server Side A4
 * Display all conflicting events
 */
app.get('/getConflicts', function(req, res) {
    var query = "SELECT * FROM EVENT EVENT WHERE start_time IN (SELECT start_time FROM EVENT GROUP BY start_time HAVING COUNT(*) >= 2)";

    connection.query(query, function(err, rows, fields) {
      res.send(rows);
    });
});

/**
 * Server Side
 * Clear all the data in the ALARM, EVENT, and FILE table in that order
 * send Success on successful completion
 */
app.get('/clearData', function (req, res) {
  var clearAlarm = "DELETE FROM ALARM;";
  var resetAlarm = "ALTER TABLE ALARM AUTO_INCREMENT=1;"
  var clearEvent = "DELETE FROM EVENT;";
  var resetEvent = "ALTER TABLE EVENT AUTO_INCREMENT=1";
  var clearFile = "DELETE FROM FILE;";
  var resetFile = "ALTER TABLE FILE AUTO_INCREMENT=1";

  connection.query(clearAlarm, function(err, result) { //attempt to clear the rows in the ALARM table
    if(err) {
      console.log("Clear alarm error" + err);
      res.send("Error");
    } else {
      connection.query(resetAlarm, function(err, result) {
        if(err) {
          console.log("Reset alarm error " + err);
          res.send("Error");
        } else {
          connection.query(clearEvent, function(err, result) { //attempt to clear the rows in the EVENT table
            if(err) {
              console.log("Clear event error" + err);
              res.send("Error");
            } else {
              connection.query(resetEvent, function(err, result){
                if (err) {
                  console.log("Reset event error " + err);
                  res.send("Error");
                } else {
                  connection.query(clearFile, function(err, result) { //attempt to clear the rows in the FILE table
                    if (err) {
                      console.log("Clear file Error" + err);
                      res.send("Error");
                    } else {
                      connection.query(resetFile, function(err, result) {
                        if(err) {
                          console.log("Reset File Error " + err);
                          res.send("Error");
                        } else {
                          console.log("Successfully cleared and reset the tables");
                          res.send("Success");
                        }
                      });
                    }
                  });
                }
              });
            }
          });
        }
      });
    }
  });
});

/**
 * Server Side A4
 */
app.get('/numEvents', function(req, res) {
    var query = "SELECT COUNT(organizer) AS count FROM EVENT WHERE organizer = '' OR organizer = 'undefined' OR organizer = 'null'";

    connection.query(query, function(err, rows, fields) {
      if(err) {
        console.log(err);
        res.send("Error");
      } else {
        res.send(rows);
      }
    })
});

/**
 * Server Side A4
 * Get all events that do not have an summary value
 */
app.get('/emptySummary', function(req, res) {
  var query = "SELECT * FROM EVENT WHERE summary = ''";

  connection.query(query, function(err, rows, fields) {
    res.send(rows);
  })
})

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);