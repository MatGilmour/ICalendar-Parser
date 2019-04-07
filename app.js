'use strict'

// C library API
const ffi = require('ffi');

let sharedLib = ffi.Library('./sharedLib', {
  'createJSONCal': [ 'string', ['string']],
  'createEventList': [ 'string', ['string']],
  'createAlarmList': [ 'string', ['string', 'int']],
  'createPropList': [ 'string', ['string', 'int']],
  'newEvent': [ 'void', [ 'string', 'string']],
  'newCal': [ 'void', ['string', 'string']],
});

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

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

app.get('/getCals', function(req, res){

  let filePath = './uploads';

  let fileNames = [];
  let fileDesc = [];

  fs.readdir(filePath, function(error, calNames){

    if(error){
      console.log("Error occured while locating files in directory");
    }

    for(let index in calNames) {
      fileNames.push(calNames[index]);

      let filePass = './uploads/' + calNames[index];
      let tempString = (sharedLib.createJSONCal(filePass));

      if(tempString == 'INV'){
        continue;
      }

      fileDesc.push(tempString);
    }

    res.send({
      fileNames,
      fileDesc,
    });
  });
});

app.get('/getICSFiles', function(req, res){

  let dupCheck = false;
  let minCount = false;
  let filePath = './uploads';
  let fileName = req.query.file;

  fs.readdir(filePath, function(error, allFiles){

    if(error){
        console.log("Error occured while locating files in directory");
    }

    for(let index in allFiles){
      if(fileName === allFiles[index]){
          
          dupCheck = true;
      }
    }

    res.send({
      dupCheck,
    });
  });
});

app.get('/fileCount', function(req,res){

  let filePath = './uploads';
  let emptyCheck = false;

  fs.readdir(filePath, function(error, allFiles){

    if(error){
        console.log("Error occured while locating files in directory");
    }

    if(allFiles.length < 1){
      emptyCheck = true;
    }

    res.send({
      emptyCheck,
    });
  });
});

app.get('/dropdownChoice', function(req, res){

  let fileName = req.query.drop;
  let filePath = './uploads/' + fileName;

  let eventList = (sharedLib.createEventList(filePath));

  res.send({
    eventList,
  });
});

app.get('/eventAlarms', function(req, res){

  let fileName = req.query.file;
  let filePath = './uploads/' + fileName;
  let eventNum = req.query.evNum;

  let alarmList = (sharedLib.createAlarmList(filePath, eventNum));

  res.send({
    alarmList,
  });
});

app.get('/eventProperties', function(req, res){

  let fileName = req.query.file;
  let filePath = './uploads/' + fileName;
  let eventNum = req.query.evNum;
  
  let propList = (sharedLib.createPropList(filePath, eventNum));

  res.send({
    propList,
  });
});

app.get('/calValidate', function(req, res){

  let fileName = req.query.file;

  res.send({
    fileName,
  });
});

app.get('/addEvent', function(req, res){

  let fileName = req.query.fileName;
  let eventJSON = req.query.eventJSON;
  let filePath = './uploads/' + fileName;

  sharedLib.newEvent(filePath, eventJSON);
});

app.get('/addCal', function(req, res){

  let fileName = req.query.fileName;
  let calJSON = req.query.calJSON;
  let filePath = './uploads/' + fileName;

  sharedLib.newCal(filePath, calJSON);
});

/* 
connection.query("", function (err, rows, fields){
  if(err){}
  else{

  }
}); */

const mysql = require('mysql');
let connection;

app.get('/login', function(req,res){

  let user = req.query.user;
  let pw = req.query.password;
  let db = req.query.database;
  let badConnect = false;
  let tableCheck = "";
  let fileCheck = false;
  let eventCheck = false;
  let alarmCheck = false;
  
  /**change this later when figuring out login stuffs */
  connection = mysql.createConnection({
    host : 'dursley.socs.uoguelph.ca',
    user : user,
    password : pw,
    database : db,
  });
 
  connection.connect(function(err){
    if(err){
      badConnect = true;
      console.log("Error connecting to" + db);
    }
    else{
      connection.query("CREATE TABLE FILE (cal_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, version INT NOT NULL, prod_id VARCHAR(256) NOT NULL, PRIMARY KEY(cal_id));", function(err,rows,fields){
        if(err){fileCheck = true;}
      });
      connection.query("CREATE TABLE EVENT (event_id INT AUTO_INCREMENT, summary VARCHAR(1024), start_time DATETIME NOT NULL, location VARCHAR(60), organizer VARCHAR(256), cal_file INT NOT NULL, PRIMARY KEY(event_id), FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE);", function (err, rows, fields){
        if(err){eventCheck = true;}
      });
      connection.query("CREATE TABLE ALARM (alarm_id INT AUTO_INCREMENT, action VARCHAR(256) NOT NULL, `trigger` VARCHAR(256) NOT NULL, event INT NOT NULL, PRIMARY KEY(alarm_id), FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE);", function (err, rows, fields){
        if(err){alarmCheck = true;}
      });
    }
    if(fileCheck = true){
      tableCheck += "File table already created, ";
    }
    if(eventCheck = true){
      tableCheck += "Event table already created, ";
    }
    if(alarmCheck = true){
      tableCheck += "Alarm table already created.";
    }
    res.send({
      badConnect,
      tableCheck,
    });
  });
});

app.get('/clearTables', function(req,res){

  connection.query("DELETE FROM FILE;", function(err,rows,fields){
    if(err){console.log("Cannot delete tables");}
    else{
      connection.query("ALTER TABLE FILE AUTO_INCREMENT = 1;", function(err, rows, fields){
        if(err){console.log("Cannot reset cal_id");}
        else{
          connection.query("ALTER TABLE EVENT AUTO_INCREMENT = 1;", function(err, rows, fields){
            if(err){console.log("Cannot reset event_id");}
            else{
              connection.query("ALTER TABLE ALARM AUTO_INCREMENT = 1;", function(err, rows, fields){
                if(err){console.log("Cannot reset alarm_id");}
              });
            }
          });
        }
      });
    }
  });
});

app.get('/logout', function(req,res){

  connection.end();
  let log = "";
  res.send({
    log,
  });
});

app.get('/dbStatus', function(req,res){

  connection.query("SELECT COUNT(*) AS files FROM FILE;", function(err, rows, fields){
    if(err){console.log("No counts found");}
    else{
      let tempFiles = JSON.stringify(rows[0]);
      let tempTwo = JSON.parse(tempFiles);
      let files = tempTwo.files;

      connection.query("SELECT * FROM EVENT;", function(err, rows, fields){
        if(err){console.log("No events found");}
        else{
          let events = (rows.length);

          connection.query("SELECT * FROM ALARM;", function(err, rows, fields){
            if(err){console.log("No alarms found");}
            else{
              let alarms = rows.length;
              res.send({
                files,
                events,
                alarms,
              });
            }
          });
        }
      });
    }
  });
});

app.get('/eventsStartDate', function(req,res){

  let eventList = [];
  connection.query("SELECT * FROM EVENT ORDER BY start_time;", function(err, rows, fields){
    if(err){console.log("No events found");}
    else{
      for(let row in rows){
        let currEvent = rows[row];

        eventList.push(currEvent);
      }
      res.send({
        eventList,
      });
    }
  });
});

app.get('/eventDateAndSum', function(req,res){

  let file = req.query.file;
  let evList = [];
  let q = "SELECT event_id, summary, start_time FROM EVENT WHERE cal_file = (SELECT cal_id FROM FILE WHERE file_name = '" + file + "');";

  connection.query(q, function(err, rows, fields){
    if(err){console.log("No Events found");}
    else{
      for(let i in rows){
        let curEvent = rows[i];

        evList.push(curEvent);
      }
      res.send({
        evList,
      });
    }
  });
});


app.get('/fileTable', function(req, res){

  let filePath = './uploads';

  let fileNames = [];
  let fileDesc = [];

  fs.readdir(filePath, function(error, calNames){

    if(error){
      console.log("Error occured while locating files in directory");
    }

    for(let index in calNames) {
      fileNames.push(calNames[index]);

      let filePass = './uploads/' + calNames[index];
      let tempString = (sharedLib.createJSONCal(filePass));

      if(tempString == 'INV'){
        continue;
      }

      fileDesc.push(tempString);

      let currFile = JSON.parse(fileDesc[index]);

      connection.query("INSERT INTO FILE (file_name, version, prod_id) values (?, ?, ?);", [fileNames[index], currFile.version, currFile.prodID ], function (err, rows, fields){
        if(err){console.log('File insertion error');}
        else{
          filePath = './uploads/' + fileNames[index];
          let cEventList = (sharedLib.createEventList(filePath));
          let eventList = JSON.parse(cEventList);
          
          for(let i in eventList){
            let currEvent = eventList[i];

            let summary;
            let location;
            let organizer;

            if(currEvent.summary.length < 1){
              summary = null;
            }
            else{
              summary = currEvent.summary;
            }
            if(currEvent.location.length < 1){
              location = null;
            }
            else{
              location = currEvent.location;
            }
            if(currEvent.organizer.length < 1){
              organizer = null;
            }
            else{
              organizer = currEvent.organizer;
            }

            let tempDT = currEvent.startDT;
            let date = tempDT.date;
            date = date.substring(0, 4) + "-" + date.substring(4, date.length);
            date = date.substring(0, 7) + "-" + date.substring(7, date.length);
            let time = tempDT.time;
            time = time.substring(0, 2) + ":" + time.substring(2, time.length);
            time = time.substring(0, 5) + ":" + time.substring(5, time.length);
            let startDT = date + ' ' + time;

            connection.query("SELECT cal_id FROM FILE;", function (err, rows, fields){
              if(err){console.log("No cal_id found");}
              else{
                let cal_file = rows[index].cal_id;
                connection.query("INSERT INTO EVENT (summary, start_time, location, organizer, cal_file) values (?, ?, ?, ?, ?);", [summary, startDT, location, organizer, cal_file], function(err, rows, fields){
                  if(err){console.log("Event insertion error");}
                  else{
                    filePath = './uploads/' + fileNames[index];
                    let temp = (+i + +1);
                    let tempAlarmList = (sharedLib.createAlarmList(filePath, temp));
                    let alarmList = JSON.parse(tempAlarmList);

                    for(let j in alarmList){
                      let currAlarm = alarmList[j];

                      let trigger = currAlarm.trigger;
                      let action = currAlarm.action;

                      connection.query("SELECT event_id FROM EVENT;", function (err, rows, fields){
                        if(err){console.log("No event_id found");}
                        else{
                          let ev = rows[i].event_id;
                          let event = (+ev + +1);

                          connection.query("INSERT INTO ALARM (action, `trigger`, event) values (?, ?, ?);", [action, trigger, event], function(err, rows, fields){
                            if(err){console.log("Alarm insertion error");}
                            else{
                            }
                          });
                        }
                      });
                    }
                  }
                });
              }
            });
          }
        }
      });
    }
  });
  res.send({
    fileNames,
  });
});

