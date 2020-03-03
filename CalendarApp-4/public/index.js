
// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    /**
     * Clear the file Log table for repopulation
     */
    function clearFileTable() {
        var table = document.getElementById('FileLogTable');
        var row = table.rows.length;
        for(var i = 1; i < table.rows.length; i++) {
            table.deleteRow(i);
        }
    }

    /**
     * Event listener for the login button A4
     * Get the login info from the form, send it to the server for login to the database
     */
    $('#databaseLoginBtn').on('click', function() {
        var userName = document.getElementById('username').value;
        var password = document.getElementById('password').value;
        var databaseID = document.getElementById('databaseName').value;

        if(databaseID == "") {
            alert("Empty database ID!");
            return;
        }

        if(userName == "") {
            alert("Empty userName!");
            return;
        }

        $.ajax({
            type: 'get',
           url: 'dbLogin',
           data: {
               userName,
               password,
               databaseID
           },
           success: function(data) {
               if(data == "Error on Login") {
                   alert("Error logging in please try again!");
               } else {
                addStat("Successfully logged into the database with " + data);
               }
               
           },
           fail: function(error) {
                addStat(error);
           }
       });
        
    });

    /**
     * Event listener for the store files button A4
     *  Get all the file names from the file log panel, send to server for building and DB placement
     */
    $('#storeFiles').on('click', function() {


        $.ajax({
            type: 'get',
            url: 'clearData',
            success: function(data) {
                if( data == "Error") {
                    addStat("Error deleting the rows from all tables, try again!");
                } else {
                    addStat("Successfully deleted all the information in the tables!");
                    var fileNames = new Array();
        var table = document.getElementById('FileLogTable');
        for(var i = 1; i < table.rows.length; i++) {
            fileNames.push(table.rows[i].cells[0].textContent);
        }

        //ajax call to send the fileName array to the server for processing
        $.ajax({
            type: 'get',
            url: 'dbStore',
            data: {
                fileNames
            },
            success: function(data) {
                if(data == "Error") {
                    addStat("Error signing into the database, try again.")
                } else {
                    addStat("Successfully added all files to the database!");
                }
                
            },
            fail: function(err) {
                addStat(err);
            }
        });
                }
            }
        });


        

    });

    /**
     * Event listener for the clear data button A4
     * Clears the tables in the DB, does not drop the tables
     */
    $('#clearData').on('click', function () {
        $.ajax({
            type: 'get',
            url: 'clearData',
            success: function(data) {
                if( data == "Error") {
                    addStat("Error deleting the rows from all tables, try again!");
                } else {
                    addStat("Successfully deleted all the information in the tables!");
                }
            }
        });
    });

    /**
     * Event listener for the database status button
     */
    $('#displayDBStatus').on('click', function() {
        $.ajax({
            type: 'get',
            url: 'displayDB',
            success: function(data) {
                addStat("Database has " + data.cals + " files, " + data.evts + " events, and " + data.alms + " alarms.");
            }
        });
    });

    /**
     * Event listener for the show events by date A4
     */
    $('#displayEventsBtn').on('click', function() {
        $.ajax({
            type: 'get',
            url: 'displayEvts',
            success: function(data) {
                $('#A4displayTable').empty();
                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 
                
                var A4Table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    
                    var startDate, summary, eventID;

                    startDate = data[i].start_time;
                    summary = data[i].summary;
                    eventID = data[i].event_id;

                    var line = "Start Date: " + startDate + " Summary: " + summary;

                    A4Table.rows[i].cells[0].innerHTML = line;
                    
                }
                
            },
            fail: function(err) {
                console.log(err);
            }
        })
    });

    /**
     * Event listener for the show events in cal A4
     */
    $('#displayCalEvent').on('click', function() {
        var file  = document.getElementById('displayEvtCalIn').value;
        var fileNames = [];

        //get all files on the fileLog panel
        var table = document.getElementById('FileLogTable');
        for(var x = 0; x < table.rows.length; x++) {
            fileNames.push(table.rows[x].cells[0].textContent);
        }

        

        if(file.includes(".ics") != true) {
            alert("Invalid File Name! (fileName.ics)");
            return;
        } else if (fileNames.includes(file) != true) {
            alert("Invalid File! File Does Not Exist!");
            return;
        }

        $.ajax({
            type: 'get',
            url: 'getCalEvt',
            data: {
                file
            },
            success: function(data) {
                $('#A4displayTable').empty(); //clear the display table
                
                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 

                var A4Table = document.getElementById("A4displayTable");
                for(var j =0; j < data.length; j++) {
                    var startD, summary;

                    startD = data[j].start_time;
                    summary = data[j].summary;

                    var line = file + " StartDate: " + startD + " Summary: " + summary;

                    A4Table.rows[j].cells[0].innerHTML = line;
                }
            },
            fail: function(err) {
                addStat(err);
            }
        });
    });

    /**
     * Event listener for the get conflicts
     */
    $('#displayConflictBtn').on('click', function() {
        $.ajax({
            type: 'get',
            url: 'getConflicts',
            success: function(data) {
                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 

                var A4Table = document.getElementById("A4displayTable");
               for(var i =0; i < data.length; i++) {
                   var sum, org;

                   sum = data[i].summary;
                   org = data[i].organizer;

                   var line = "Conflicting Event with summary: " + sum + " and organizer: " + org;

                    A4Table.rows[i].cells[0].innerHTML = line;

               }
            },
            fail: function(err) {
                console.log(err);
            }
        });
    });

    /**
     * Event listener for getting all events without a summary value
     * 
     */
    $('#eventSumBtn').on('click', function() {
        $.ajax({
            type: 'get',
            url: 'emptySummary',
            success: function(data) {
                $('#A4displayTable').empty();
                console.log(data.length);

                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 

                var A4Table = document.getElementById("A4displayTable");
                for(var j =0; j < data.length; j++) {
                    
                    var startD, calId, eventID;

                    startD = data[j].start_time;
                    
                    calId = data[j].cal_file;
                   
                    eventID = data[j].event_id;
                    
                    
                    
                    

                    var line = "Event " + eventID + " with a StartDate: " + startD + " in Calendar: " + calId + " has an empty summary";

                    A4Table.rows[j].cells[0].innerHTML = line;
                }
            }
        });
    });

    /**
     * Event listener for getting num events in a specific calendar
     */
    $('#evtCountBtn').on('click', function() {
        //var location = document.getElementById('evtCountIN').value;

        $.ajax({
            type: 'get',
            url: "numEvents",
            success: function(data) {
                console.log("DATA");
                console.log(data);
                $('#A4displayTable').empty();
                console.log(data.length);

                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 

                var A4Table = document.getElementById("A4displayTable");
                for(var j =0; j < data.length; j++) {
                    
                    var num = data[j].count;
                    

                    var line = "There are  " + num + " Events that have empty or null/undefined organizers";

                    A4Table.rows[j].cells[0].innerHTML = line;
                }
            },
            fail: function(error) {
                console.log("Error");
            }
        });
    });

    /**
     * Event listener for getting the distinct alarm values based on trigger
     */
    $('#displayAlarmsBtn').on('click', function() {
        $.ajax({
            type: 'get',
            url: 'getFistinctAlarm',
            success: function(data) {
                $('#A4displayTable').empty();
                console.log(data.length);

                var table = document.getElementById("A4displayTable");
                for(var i = 0; i < data.length; i++) {
                    var length = table.rows.length;

                //Create an empty <tr> element and add it to the top position of the table
                var row = table.insertRow(length);

                //Add to the fileName cell
                var stat = row.insertCell(0);
                } 

                var A4Table = document.getElementById("A4displayTable");
                for(var j =0; j < data.length; j++) {
                    
                    var trig;

                    trig = data[j].trigger;
                    
                    
                    
                    
                    
                    

                    var line = trig;

                    A4Table.rows[j].cells[0].innerHTML = line;
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    /**
     * Display the Store Files button
     */
    function checkFilePanel() {
        
        var table = document.getElementById("FileLogTable");

        var rows = table.rows.length;
        console.log("Rows:" + rows);
        if(rows > 1) {
            var storeBtn = document.getElementById('storeFiles');
            storeBtn.style.visibility = 'visible';
        }
        
        
        console.log("checking for empty filelog table");
    }


    /**
     * Determine if file panel has rows
     */
    function checkFP() {
        var table = document.getElementById("FileLogTable");

        var rows = table.rows.length;
        console.log("Rows:" + rows);
    }
    

    /**
     * Adds the message for when the server contains no files
     */
    function emptyFileList() {
        
        var fileTable = document.getElementById("FileLogTable");

        var row = fileTable.insertRow(1);

        var fileName = row.insertCell(0);
        var version = row.insertCell(1);
        var prodID = row.insertCell(2);
        var events = row.insertCell(3);
        var props = row.insertCell(4);

        fileName.innerHTML = "No files";
    }

    /**
     * Function to fill the actual file info from the server
     * File list does not contain any invalid files!
     */
    function fillFileInfo() {
        /**
         * AJAX for getting the JSON of a calendar, also fills the empty table cells with the info
         */
        $.ajax( {
            type: 'get',
            url: '/fileToJSon',
            success: function(data) {
                var fileTable = document.getElementById("FileLogTable");
                var obj, version, prodID, numEvents, numProps;
                for(var i = 0; i < data.length; i++) {
                    obj = JSON.parse(data[i]);
                    version = obj.version;
                    prodID = obj.prodID;
                    numEvents = obj.numEvents;
                    numProps = obj.numProps;
                    fileTable.rows[i + 1].cells[1].innerHTML = version;
                    fileTable.rows[i + 1].cells[2].innerHTML = prodID;
                    fileTable.rows[i + 1].cells[3].innerHTML = numEvents;
                    fileTable.rows[i + 1].cells[4].innerHTML = numProps;
                }
                populateCalList();
            },
            fail: function() {
                console.log("Error!");
            }
        });
    }

    /**
     * AJAX for checking if the uploads folder is empty
     * Will also fill the table with the fileNames if there are any
     */
    function loadFileTable() {

    
    $.ajax( {
        type: 'get', //type of ajax call
        url: '/uploads', //this is the server end point to connect to (is user defined in app.js) 
        success: function(data) { //on success, do something to the tables
            if(!data.length) {
                //console.log("Empty uploads");
                var fileT = document.getElementById('"FileLogTable');
                emptyFileList(); //empty the file list
            } else {
                var i = data.length - 1;

                var fileTable = document.getElementById("FileLogTable");
                //loop over the data returned and add the file name to the file log panel
                for(i; i > 0; i--) {
                    //console.log(data[i]);
                    //if the filename contains the .ics extension display it
                    if(data[i].includes(".ics")) {
                        makeFileTable(data[i]);
                    }
                }
                fillFileInfo();
                addStat("Added files from the server");
            }

            checkFilePanel();
        },
        fail: function(error) {
            addStat(error + "when trying to fill the file table!");
            //Non-200 return we will print the error type out to the status panel
            console.log("Failure my dude");
        }
    });
    } 
    
    //fill the file log panel on load
    window.onload = loadFileTable();
    window.onload = checkFilePanel(); //check if the filepanel is empty
    
    /**
     * iterate over the filelog panel, getting the FileName store them in a string
     * 
     * use .split function
     */
    function populateCalList() {
        var theTable = document.getElementById('FileLogTable');
        var calNames = "";
        var opt = document.createElement('option');
        var selecter = document.getElementById('calViewDrop');

        //reset the selecter
        selecter.options.length = 0;

        for(var i = theTable.rows.length-1; i > 0; i--) {
            calNames += (theTable.rows[i].cells[0].textContent) + ",";
        }

        //console.log(calNames);

        //populate an array with the cal files
        calArr = calNames.split(',');

        //clear the selector
        selecter.innerHTML = "";

        //iterate over the array adding each element to the selecter
        for(var j = 1; j < calArr.length -1; j++) {
            var option = document.createElement("option");
            var temp = calArr[j];
            option.textContent = temp;
            option.value = temp;
            selecter.add(option);
        }

    }

    
    /**
     * Upon clicking the calViewDrop, gets the list of events from the server
     * Fills the Calendar View Panel with all the events in the calendar file
     */
    $('#calViewDrop').on('click', function() {
        var select = document.getElementById('calViewDrop');
        var value = "uploads/"+  select.value;
    /**
     * AJAX for getting the JSON of a calendar's event list
     * Fills the Calendar view panel based on the JSON
     */
    $.ajax({
             type: 'get',
            url: 'getEvtList',
            data: {
                value
            },
            success: function(data) {
                var calTable = document.getElementById('calViewTable');
                clearCalview(); //clear the calendar view table before repopiulating
                //console.log(data);
                
                var dateTime;
                var JSonArray = JSON.parse(data);
                
                for(var i = 0; i < JSonArray.length; i++) {
                    dateTime = JSonArray[i];
                var startDate = dateTime.startDT.date.slice(0, 4) + "/" + dateTime.startDT.date.slice(5, 6) + "/" + dateTime.startDT.date.slice(7, 8);
                var startTime = dateTime.startDT.time.slice(0,2) + ":" + dateTime.startDT.time.slice(2,4) + ":" + dateTime.startDT.time.slice(4,7);
                    if(dateTime.startDT.isUTC == true) {
                        startTime = startTime + " (UTC)";
                    }
                
               
                var sum = dateTime.summary;
                var props = dateTime.numProps;
                var alarms = dateTime.numAlarms;
                    
                addCalView(i + 1, startDate, startTime, sum, props, alarms);
                }
                
            },
            fail: function(error) {
                 addStat(error);
            }
        });
        
        
    });

    /**
     * Upon click, displays the form that corresponds to the query that the user wants
     */
    $('#queryDropDown').on('click', function() {
        var select = document.getElementById('queryDropDown');
        var value = select.value;
        console.log(value);
        if(value == "Display Events") {
            //set selected form to visible
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'block';

            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'none';
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'none';
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'none';
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'none';
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'none';

        }else if (value == "Display Events from File") {
            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'block';
            

            //reset all other forms to be invisible
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'none';
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'none';
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'none';
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'none';
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'none';

        }else if (value == "Display Conflicting Events") {
            //set selected form to visible
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'block';
            

            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'none';
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'none';
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'none';
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'none';
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'none';
            
        }else if (value == "Display Distinct Trigger Value") {
            //set selected form to visible
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'block';
            

            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'none';
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'none';
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'none';
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'none';
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'none';
            
        }else if (value == "Get Events without Summary") {
            //set selected form to visible
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'block';
            

            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'none';
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'none';
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'none';
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'none';
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'none';
            
        }else if (value == "Get Events with empty Organizer") {
            //set selected form to visible
            var avgEventForm = document.getElementById('avgEventForm');
            avgEventForm.style.display = 'block';
            

            //reset all other forms to be invisible
            var displayCalEventForm = document.getElementById('displayCalEventForm');
            displayCalEventForm.style.display = 'none';
            var form = document.getElementById('displayEventsForm');
            form.style.display = 'none';
            var displayConfilctForm = document.getElementById('displayConfilctForm');
            displayConfilctForm.style.display = 'none';
            var eventDatesForm = document.getElementById('eventSummaryForm');
            eventDatesForm.style.display = 'none';
            var displayAlarmsForm = document.getElementById('displayAlarmsForm');
            displayAlarmsForm.style.display = 'none';
            
        }
    });

    /**
     * Utility function to add a row to the Calendar View Panel
     */
    function addCalView(theEvent, sDate, sTime, summary, numProps, numAlarms) {
        var calTable = document.getElementById('calViewTable');
        var row = calTable.insertRow(1);

        var eventNum = row.insertCell(0);
        var startDate = row.insertCell(1);
        var startTime = row.insertCell(2);
        var summCell = row.insertCell(3);
        var props = row.insertCell(4);
        var alarms = row.insertCell(5);

        eventNum.innerHTML = theEvent;
        startDate.innerHTML = sDate;
        startTime.innerHTML = sTime;
        summCell.innerHTML = summary;
        props.innerHTML = numProps;
        alarms.innerHTML = numAlarms;


    }

    /**
     * Utility function to clear the Calendar View Panel
     */
    function clearCalview() {
        var table = document.getElementById('calViewTable');
        

        for(var i = 1; i < table.rows.length; i++) {
            table.rows[i].innerHTML = "";
        }
    }
    

    /**
     * This should allow a file upload on the submit of the form containing the upload btn
     */
    //var uploadEvt = document.getElementById('uploadBtn').addEventListener('click', sendFile);


    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Callback from the form");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the 
        $.ajax({});

    });
    
    //var makeCal = document.getElementById('createCalBtn').addEventListener('click', getCalInfo);
    //Event Listener for the clearStatusBtn
    let clearBtn = document.getElementById('clearStatBtn').addEventListener('click', clearStat);

    //Event listener for the create Cal button
    let createCalEvt = document.getElementById('showCalFormBtn');
    createCalEvt.addEventListener("click", displayCalMenu);

    //Event listener forn the Create Event button
    let createEvt = document.getElementById('showEvtBtn');
    createEvt.addEventListener("click", displayEvtMenu);

    
    /**
     * Upon creating pressing createCal, sends new cal information to server for processing
     * Upon success, parses the returned JSON and adds it to the fileLogPanel
     * DONE
     */
    $('#creatCalBtn').on('click', function() {
        var fileName = document.getElementById('calFileName').value;
        var version = document.getElementById('version').value;
        var prodID = document.getElementById('prodID').value;
        var calSDT = document.getElementById('calSDT').value;
        var calSTT = document.getElementById('calendarStartTime').value;
        var calSum = document.getElementById('calSummary').value;
        var evtUID = document.getElementById('UID').value;

        //Check for valid extension
        if(!fileName.includes(".ics")) {
            addStat('Invalid fileName: No .ics extension');
            return;
        }

        //check if the version is a number or not
        if(isNaN(version)) {
            addStat("Version needs to be a number!");
            return;
        }

        //check if the user entered a valid date length
        if(calSDT.length < 14 || calSDT.length > 16) {
            addStat("Calendar start date field is too long or too short!");
            return;
        }

        //check if the T seperator is included
        if(!calSDT.includes("T")) {
            addStat("Calendar start date field missing T seperator!");
            return;
        }

        //check if the user entered a valid date length
        if(calSTT.length < 14 || calSDT.length > 16) {
            addStat("Calendar start time field is too long or too short!");
            return;
        }

        //check if the T seperator is included
        if(!calSTT.includes("T")) {
            addStat("Calendar start time field missing T seperator!");
            return;
        }

        //check for filled in required fields
        if(prodID === "") {
            addStat("ProdID is required field!");
            return;
        }

        if(calSDT === "") {
            addStat("Calendar start date is required!");
            return;
        }

        if(calSTT === "") {
            addStat("Calendar start time is required!");
            return;
        }

        if(evtUID === "") {
            addStat("UID is required!");
            return;
        }

        if(version === "") {
            addStat("Version is required!");
        }

        //check for too large of prodID
        if(prodID.length > 1000) {
            addStat("ProdID must be less than 1000 characters!");
            return;
        }

        if(evtUID.length > 1000) {
            addStat("UID must be less than 1000 characters!");
            return;
        }

        //Iterate over the fileLogTable's fileName column checking for dupes
        var fileTable = document.getElementById('FileLogTable');
        for(var i = 1; i <  fileTable.rows.length; i++) {
            if(fileName == fileTable.rows[i].cells[0].textContent) {
                addStat('Duplicate file name, please enter a new file name');
                return;
            }
        }

        $.ajax ({
            type: 'get',
            url: '/sendCalInfo',
            data: {
                fileName,
                version,
                evtUID,
                prodID,
                calSDT,
                calSTT,
                calSum
            },
            //upon success returns a JSon representing a new calendar to be added to the fileLogPanel
            success: function (data) {
                
                var fileTable = document.getElementById("FileLogTable");
                var obj, version, prodID, numEvents, numProps;
            
                obj = JSON.parse(data);
                var row = fileTable.insertRow(1);
                var fileNameCell = row.insertCell(0);
                var versionCell = row.insertCell(1);
                var prodIDCell = row.insertCell(2);
                var events = row.insertCell(3);
                var props = row.insertCell(4);

                
                version = obj.version;
                prodID = obj.prodID;
                numEvents = obj.numEvents;
                numProps = obj.numProps;
                fileNameCell.innerHTML = '<a href="/uploads/' + fileName + '">' + fileName + '</a>';
                versionCell.innerHTML = version;
                prodIDCell.innerHTML = prodID;
                events.innerHTML = numEvents;
                props.innerHTML = numProps;

                populateCalList();
   
                addStat(fileName + " has been added!");
            },
            fail: function (error) {
                addStat(error);
            }
        });
    });


    /**
     * Creates a new event for the selected calendar in the drop down
     * Upon seccess, update the status panel with a success message
     * DONE
     */
    $('#createEvtBtn').on('click', function() {
        var dropDown = document.getElementById('evtDropDown');
        var fileName = "uploads/" + dropDown.value; //get the currently selected member of the dropdown
        
        //user entered values
        var theUid = document.getElementById('theUid').value;
        var startDate = document.getElementById('evtSDT').value;
        var startTime = document.getElementById('evtST').value;
        var summary = document.getElementById('evtSum').value;
        //console.log(summary);

        //check for empty required fields
        if(theUid === "") {
            addStat("UID field is empty!");
            return;
        }

        if(startDate === "") {
            addStat("Start Date is empty!");
            return;
        }

        if(startTime === "") {
            addStat("Start Time is empty!");
            return;
        }

        //check if the user entered a valid date length
        if(startDate.length < 14 || startDate.length > 16) {
            addStat("Calendar start date field is too long or too short!");
            return;
        }

        //check if the UID is too long
        if(theUid.length > 1000) {
            addStat("UID is too long!");
            return;
        }

        //check if the T seperator is included
        if(!startDate.includes("T")) {
            addStat("Calendar start date field missing T seperator!");
            return;
        }

        //check if the user entered a valid date length
        if(startTime.length < 14 || startTime.length > 16) {
            addStat("Calendar start time field is too long or too short!");
            return;
        }

        //check if the T seperator is included
        if(!startTime.includes("T")) {
            addStat("Calendar start time field missing T seperator!");
            return;
        }

        /**
         * AJAX to add a new event to a calendar
         * Sends new evt info to the server
         */
        $.ajax({
            type: 'get',
            url: '/addEvt',
            data: {
                fileName,
                theUid,
                startDate,
                startTime,
                summary
            },
            success: function(data) {
                clearFileTable(); //clear the file table
                loadFileTable(); //get the file names
                fillFileInfo(); //get the actual file info
                populateCalList();
                addStat("Successfully added a new event to " + dropDown.value);
            },
            fail: function(data) {

            }
        });
    });

    /**
     * Get the list of properties in the desired event
     */
    $('#showPropBtn').on('click', function() {
        var cal = "uploads/" + document.getElementById('calViewDrop').value;
        var event = document.getElementById('findEvtNum').value;

        //if user enters 0 thats invalid
        if(event == 0) {
            addStat("Please enter a valid event number!");
            return;
        }

        var list = document.getElementById('calViewTable').rows.length;
        if(event > list - 1) {
            addStat(event + " does not exist!");
            return;
        }
        
        $.ajax({
            type: 'get',
            url: '/getPropList',
            data: {
                cal,
                event
            },
            success: function(data) {
                clearStat();
                var JSonArray = JSON.parse(data);
                for(var i = 0; i < JSonArray.length; i++) {
                    var prop = JSonArray[i];
                    addStat("Name: " + prop.propName + "  Desc: " + prop.propDescr);
                }
            },
            fail: function(error) {
                addStat(error);
            }
        })
    });

    /**
     * Get the list of alarms in the desired event 
     */
    $('#showAlarmBtn').on('click', function() {
        var cal = "uploads/" + document.getElementById('calViewDrop').value; //get the calendar to look in
        var event = document.getElementById('findEvtNum').value; //get the event we want to look in
        
        //if user enters 0 thats invalid
        if(event == 0) {
            addStat("Please enter a valid event number!");
            return;
        }

        var list = document.getElementById('calViewTable').rows.length;
        if(event > list - 1) {
            addStat(event + " does not exist!");
            return;
        }

        $.ajax({
            type: 'get',
            url: '/getAlarmList',
            data: {
                cal,
                event
            },
            success: function(data) {
                clearStat();
                var JSonArray = JSON.parse(data);
                for(var i =0; i < JSonArray.length; i++) {
                    var alarm = JSonArray[i];
                    addStat("Action: " + alarm.action + "   Trigger: " + alarm.trigger + "   Num Props: " + alarm.propNum);
                } 
            },
            fail: function(error) {
                addStat(error);
            }
        });
    }); 


    /**
     * Clear the status table
     */
    function clearStat() {
        $('#StatusTable').empty();
    }

    /**
     * Add a status message to the table
     */
    function addStat(theMessage) {
        //get the table
        var statusTable = document.getElementById("StatusTable");
        var length = statusTable.rows.length;

        //Create an empty <tr> element and add it to the top position of the table
        var row = statusTable.insertRow(length);

        //Add to the fileName cell
        var stat = row.insertCell(0);

        stat.innerHTML = theMessage;
    }

    /**
     * Get a file to uplaod to the server
     */
    $('#uploadBtn').on('click', function(){

        alert("Uploading is disabled, have a bug that I can't seem to find!");
        /*
        var uploadFile = document.getElementById('fileInput').files[0];
        var uploadFiles = document.getElementById('fileInput').files;

        console.log(uploadFile);
        console.log(uploadFiles);

        if(uploadFile.name == "megaCal.ics" || uploadFile.name == "megaCal1.ics" || uploadFile.name == "sourceCa.ics") {
            alert("Cannot upload file due to bug please try a different file!");
            return;
        }
        

        if (uploadFiles.length == 0) {
            //print some message for empty upload
        } else {

            //Create a new FormData() object and attach the file to it
            let files = new FormData();
            files.append("uploadFile", uploadFile); //get the file

            //Get the files from the server
            $.ajax({
                 type: 'post',
                 url: '/upload',
                 data: files,
                 processData: false,
                 contentType: false,

                 //Validate the newly uploaded file
                 success: function(data) {
                     //console.log(data);
                 },

                 error: function(error) {
                     //console.log(data);
                 }
            });
            $('#calViewDrop').empty();
            $('#evtDropDown').empty();
            clearFileTable();
            loadFileTable();
        }
        */
        
    });
    
    /**
     * fill the file table with names and empty info cells 
     */
    function makeFileTable (fileN) {
        var fileTable = document.getElementById("FileLogTable");

        var row = fileTable.insertRow(1);

        var fileName = row.insertCell(0);
        var version = row.insertCell(1);
        var prodID = row.insertCell(2);
        var events = row.insertCell(3);
        var props = row.insertCell(4);

        fileName.innerHTML = '<a href="/uploads/' + fileN + '">' + fileN + '</a>';
    }

    /**
     * Populate the event drop down with the calendar files
     */
    function popEvtList() {
        var theTable = document.getElementById('FileLogTable');
        var calNames = "";
        var opt = document.createElement('option');
        var selecter = document.getElementById('evtDropDown');

        //reset the selecter
        selecter.options.length = 0;

        for(var i = theTable.rows.length -1; i > 0; i--) {
            calNames += "," + theTable.rows[i].cells[0].textContent;
        }

        //populate an array with the cal files
        calArr = calNames.split(',');

        //clear the selector
        selecter.innerHTML = "";

        //iterate over the array adding each element to the selecter
        for(var j = 1; j < calArr.length; j++) {
            var option = document.createElement("option");
            var temp = calArr[j];
            option.textContent = temp;
            option.value = temp;
            selecter.add(option);
        }

        
    }

    /**
     * Button to display the create event form
     */
    function displayEvtMenu() {
        let evtForm = document.getElementById('createEvtForm');
        evtForm.style.display = "block";

        popEvtList();
    }

    /**
     * Button to display the create calendar form
     */
    function displayCalMenu () {
        let calMenu = document.getElementById('createCalForm').style.display = "block";
    }
  

    $('#createCalBtn').on('click', function(){
        console.log("Creat cal button in the form has been presssed!");
    });
    

    
});