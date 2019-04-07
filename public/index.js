function populateTable() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getCals',
        success: function (data) {
            
            let fileLog = '<tr id="tags"><th><b>file name<br>(click to download)</b></th><th><b>Version</b></th><th><b>Product ID</b></th><th><b>Number of events</b></th><th><b>Number of properties</b></th></tr>';
            let dropDown = '<option>Choose Calendar</option>';

            if(data.fileNames.length === 0){
                fileLog += '<tr><th colspan="6">No files</th></tr>';
            }
            else {
                let nameArray = data.fileNames;
                let descArray = data.fileDesc;

                for (let index in nameArray) {
                    let currArray = JSON.parse(descArray[index]);

                    fileLog += '<tr><th><a href="/uploads/' + nameArray[index] + '">' + nameArray[index] + '</a></th><th>' + currArray.version + '</th><th>' + currArray.prodID + '</th><th>' + currArray.numEvents + '</th><th>' + currArray.numProps + '</th></tr>';

                    dropDown += '<option>' + nameArray[index] + '</option>';
                }
            }

            $('#fileLog').html(fileLog);
            $('#dropDown').html(dropDown);
            $('#calList').html(dropDown);
        },
        fail: function (error) {
            console.log(error);
        }
    });
}

function populateCalView(eventList) {

    let calView = '<tr id="tags"><th><b>Event No</b></th><th><b>Start date</b></th><th><b>Start time</b></th><th><b>Summary</b></th><th><b>Props</b></th><th><b>Alarms</b></th></tr>';

    if (eventList.length < 1) {
        $('#calView').html(calView);
    }
    else {
        for (let i in eventList) {

            let startDT = eventList[i].startDT;

            let date = startDT.date;
            let time = startDT.time;
            let desc = "";
            let UTC = "";

            if (eventList[i].summary == '') {
                desc = "";
            }
            else {
                desc = eventList[i].summary;
            }

            if (startDT.isUTC === true) {
                UTC = " (UTC)";
                time += UTC;
            }

            date = date.insert(4, "/");
            date = date.insert(7, "/");
            time = time.insert(2, ":");
            time = time.insert(5, ":");

            calView += '<tr><th>' + (+i + +1) + '</th><th>' + date + '</th><th>' + time + '</th><th>' + desc + '</th><th>' + eventList[i].numProps + '</th><th>' + eventList[i].numAlarms + '</th></tr>';
        }
        $('#calView').html(calView);
    }
}

function displayAlarms(alarmList){
    let alarmString = '';

    if(alarmList.length === 0){
        alarmString = 'No Alarms Present';
    }
    else {
        for(let i in alarmList){
            let currAlarm = alarmList[i];

            alarmString += 'Alarm ' + (+i + +1) + ': <br>Action: ' + currAlarm.action + '<br>Trigger: ' + currAlarm.trigger + '<br>propNum: ' + currAlarm.propNum + '<br><br>';
        }
    }

    $('#status').html(alarmString);
    document.getElementById('status').style.color = '#000000';
}

function displayProperties(propList){

    let propString = '';

    if(propList.length === 0){
        propString = 'No Properties Present';
    }
    else {
        for(let i in propList){
            let currProp = propList[i];

            propString += 'Property ' + (+i + +1) + ': <br>Name: ' + currProp.name + '<br>Description: ' + currProp.desc + '<br><br>';
        }
    }
    
    $('#status').html(propString);
    document.getElementById('status').style.color = '#000000';
}

function verifyICS(fileName) {

    let icsCheck = false;

    if (fileName.match(/\.[0-9a-z]+$/i) == '.ics') {
        icsCheck = true;
    }
    else {
    }

    return icsCheck;
}

function findDuplicate(fileData) {

    let fileName = fileData.name;

    $.ajax({
        type: 'get',
        url: '/getICSFiles',
        data: {
            file: fileName,
        },
        success: function (data) {
            if (data.dupCheck === true) {
                $('#status').html("Duplicate found");
                document.getElementById('status').style.color = '#AA0000';
            }
            else {

                $('#status').html("File uploaded successfully");
                document.getElementById('status').style.color = '#00BB00';

                let formData = new FormData();
                formData.append('uploadFile', fileData);

                $.ajax({
                    url: '/upload',
                    type: 'POST',
                    data: formData,
                    processData: false,  // tell jQuery not to process the data
                    contentType: false,  // tell jQuery not to set contentType
                    success: function (data) {
                        populateTable();
                    },
                    fail: function (error) {
                        console.log(error);
                    }
                });
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });
}

/*$.ajax({
    type: 'get',
    url: '/',
    data: {

    },
    success: function (data) {

    },
    fail: function(data) {
        console.log(error);
    }
});*/
function storeFiles(){

    $.ajax({
        type: 'get',
        url: '/fileCount',
        success: function (data) {
            if (data.emptyCheck === true) {
                $('#status').html("No files found");
                document.getElementById('status').style.color = '#AA0000';
            }
            else{
                $.ajax({
                    type: 'get',
                    url: '/fileTable',
                    success: function (data) {
                        updateDB();
                    },
                    fail: function (err) {
                        console.log(err);
                    }
                });
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });
}
function updateDB(){
    $.ajax({
        url: '/dbStatus',
        type: 'get',
        success: function(data){
            let currStatus = "Database has " + data.files + ' files, ' + data.events + ' events, and ' + data.alarms + ' alarms';
            $('#status').html(currStatus);
            document.getElementById('status').style.color = '#00BB00';
        },
        fail: function(err){
            console.log(err);
        }
    }); 
}

String.prototype.insert = function (index, string) {
    if (index > 0) {
        return this.substring(0, index) + string + this.substring(index, this.length);
    }
    return string + this;
};

// Put all onload AJAX calls here, and event listeners
$(document).ready(function () {

    populateTable();
    let eventList = '';
    populateCalView(eventList);

    $('#createEv').hide(); 
    $('#close').hide();
    $('#createCal').hide();
    $('#closeCal').hide();
    $('#userLogin').show();
    $('#query').hide();
    $('#queryList').hide();

    $('#clear').on('click', function () {
        $('#status').html("No Current Status...");
        document.getElementById('status').style.color = '#000000';
    });

    $('#eventText').focus(function () {
        if ($('#eventText').val() === 'Please select a valid event number...') {
            $('#eventText').val("");
        }
    }).blur(function () {
        if ($('#eventText').val() === '') {
            $('#eventText').val("Please select a valid event number...");
        }
    });

    $('#propText').focus(function () {
        if ($('#propText').val() === 'Please select a valid event number...') {
            $('#propText').val("");
        }
    }).blur(function () {
        if ($('#propText').val() === '') {
            $('#propText').val("Please select a valid event number...");
        }
    });

    $('#alarmButton').on('click', function () {

        let fileName = $('#dropDown').val();
        let evChoice = $('#eventText').val();

        if (fileName === 'Choose Calendar') {
            $('#status').html("No File Was Selected");
            document.getElementById('status').style.color = '#AA0000';
        }
        else {
            $.ajax({
                url: '/eventAlarms',
                type: 'get',
                dataType: 'json',
                data: {
                    file: fileName,
                    evNum: evChoice,
                },
                success: function (data) {
                    let alarmList = JSON.parse(data.alarmList);
                    if(alarmList.error === "true"){
                        $('#status').html("Invalid Event Number");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else if(alarmList.error === "cal"){
                        $('#status').html("Invalid Calendar");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else {
                        displayAlarms(alarmList);
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
    });

    $('#propButton').on('click', function() {

        let fileName = $('#dropDown').val();
        let evChoice = $('#propText').val();

        if (fileName === 'Choose Calendar') {
            $('#status').html("No File Was Selected");
            document.getElementById('status').style.color = '#AA0000';
        }
        else {
            $.ajax({
                url: '/eventProperties',
                type: 'get',
                dataType: 'json',
                data: {
                    file: fileName,
                    evNum: evChoice,
                },
                success: function (data) {
                    let propList = JSON.parse(data.propList);
                    if(propList.error === "true"){
                        $('#status').html("Invalid Event Number");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else if(propList.error === "cal"){
                        $('#status').html("Invalid Calendar");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else{
                        displayProperties(propList);
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
    });

    $('#filechooser').on('change', function (e) {

        let fileName = e.target.files[0].name;
        let fileData = e.target.files[0];

        let ics = verifyICS(fileName);

        if (ics === true) {
            let dup = findDuplicate(fileData);
        }
        else {
            $('#status').html("Incorrect file format (File must be .ics format)");
            document.getElementById('status').style.color = '#AA0000';
        }
    });

    $('#dropDown').on('change', function () {

        let dropDown = $('#dropDown').val();

        if (dropDown !== 'Choose Calendar') {
            $.ajax({
                url: '/dropdownChoice',
                type: 'get',
                dataType: 'json',
                data: {
                    drop: dropDown
                },
                success: function (data) {
                    let eventList = JSON.parse(data.eventList);
                    if(eventList.error === "cal"){
                        $('#status').html("Invalid Calendar");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else{
                        populateCalView(eventList);
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        else {
            let calView = '<tr id="tags"><th><b>Event No</b></th><th><b>Start date</b></th><th><b>Start time</b></th><th><b>Summary</b></th><th><b>Props</b></th><th><b>Alarms</b></th></tr>';
            $('#calView').html(calView);
        }
    });

    $('#eventButton').on('click', function(){

        $('#createEv').show(); 
        $('#close').show();
        $('#createCal').hide();
        $('#closeCal').hide();
        $('#userFile').val("");
        $('#version').val("");
        $('#prodID').val("");
        $('#timeTextCal').val("");
        $('#dateTextCal').val("");
        $('#uidCal').val("");
        $('#summaryCal').val("");
    });

    $('#calButton').on('click', function(){

        $('#createCal').show();
        $('#closeCal').show();
        $('#createEv').hide();
        $('#close').hide();
        $('#timeText').val("");
        $('#dateText').val("");
        $('#uid').val("");
        $('#summary').val("");
    });

    $('#closeButton').on('click', function(){

        $('#createEv').hide();
        $('#close').hide();
        $('#timeText').val("");
        $('#dateText').val("");
        $('#uid').val("");
        $('#summary').val("");
    });

    $('#closeBtn').on('click', function(){

        $('#createCal').hide();
        $('#closeCal').hide();
        $('#userFile').val("");
        $('#version').val("");
        $('#prodID').val("");
        $('#timeTextCal').val("");
        $('#dateTextCal').val("");
        $('#uidCal').val("");
        $('#summaryCal').val("");
    });

    $('#submitButton').on('click', function(){

        let today = new Date();
        let fileName = $('#dropDown').val();

        //startTime
        let sTime = $('#timeText').val();
        let temp = sTime.slice(0,2) + sTime.slice(3,sTime.length);
        let startTime = temp.slice(0,4) + temp.slice(5,temp.length);
        startTime += '00';

        //startDate
        let sDate = $('#dateText').val();
        let temp2 = sDate.slice(0,4) + sDate.slice(5, sDate.length);
        let startDate = temp2.slice(0,6) + temp2.slice(7,temp2.length);

        //UID and summary
        let uid = $('#uid').val();
        let summary = $("#summary").val();

        //currDate
        let tempDate = today.getFullYear() + '-' + 0 + (today.getMonth()+1) + '-' + today.getDate();
        let temp3 = tempDate.slice(0,4) + tempDate.slice(5, tempDate.length);
        let currDate = temp3.slice(0,6) + temp3.slice(7,temp3.length);
        
        //currTime
        let hours = today.getHours();
        let hrs = "";
        if(hours < 10){
            hrs += '0' + hours;
        }
        else{
            hrs += hours;
        }

        let minutes = today.getMinutes();
        let min = "";
        if(minutes < 10){
            min += '0' + minutes;
        }
        else{
            min += minutes;
        }

        let seconds = today.getSeconds();
        let sec = "";
        if(seconds < 10){
            sec += '0' + seconds;
        }
        else{
            sec += seconds;
        }

        let tempTime = hrs + ':' + min + ':' + sec;
        let temp4 = tempTime.slice(0,2) + tempTime.slice(3,tempTime.length);
        let currTime = temp4.slice(0,4) + temp4.slice(5,temp4.length);
        
        if(sTime === ''){
            $('#status').html("Start Time Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(sDate === ''){
            $('#status').html("Start Date Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(uid === ''){
            $('#status').html("UID is missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else{
            if (fileName !== 'Choose Calendar') {
                let eventJSON = '{"startTime":"' + startTime + '","startDate":"' + startDate + '","uid":"' + uid + '","summary":"' + summary + '","createTime":"' + currTime + '","createDate":"' + currDate + '"}';
                
                $.ajax({
                    url: '/addEvent',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        eventJSON,
                        fileName,
                    },
                    success: function (data) {

                    },
                    fail: function (error) {
                        console.log(error);
                    }
                });
            }
            else {    
                $('#status').html("No File Selected");
                document.getElementById('status').style.color = '#AA0000';
            }
        }

        let tempFile = $('#dropDown').val();
        if(sTime === '' || sDate === '' || uid === '' || tempFile === 'Choose Calendar'){
        }
        else {
            let dropDown = $('#dropDown').val();
            $.ajax({
                url: '/dropdownChoice',
                type: 'get',
                dataType: 'json',
                data: {
                    drop: dropDown,
                },
                success: function (data) {
                    let eventList = JSON.parse(data.eventList);
                    if(eventList.error === "cal"){
                        $('#status').html("Invalid Calendar");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else{
                        $('#status').html("Event has been created successfully");
                        document.getElementById('status').style.color = '#00BB00';
                        populateCalView(eventList);
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
    });

    $('#submitBtn').on('click', function(){
        let today = new Date();

        //startTime
        let sTime = $('#timeTextCal').val();
        let temp = sTime.slice(0,2) + sTime.slice(3,sTime.length);
        let startTime = temp.slice(0,4) + temp.slice(5,temp.length);
        startTime += '00';

        //startDate
        let sDate = $('#dateTextCal').val();
        let temp2 = sDate.slice(0,4) + sDate.slice(5, sDate.length);
        let startDate = temp2.slice(0,6) + temp2.slice(7,temp2.length);

        //UID and summary
        let uid = $('#uidCal').val();
        let summary = $("#summaryCal").val();
        let userFile = $('#userFile').val();
        let version = $('#version').val();
        let prodID = $('#prodID').val();

        //currDate
        let tempDate = today.getFullYear() + '-' + 0 + (today.getMonth()+1) + '-' + today.getDate();
        let temp3 = tempDate.slice(0,4) + tempDate.slice(5, tempDate.length);
        let currDate = temp3.slice(0,6) + temp3.slice(7,temp3.length);
        
        //currTime
        let hours = today.getHours();
        let hrs = "";
        if(hours < 10){
            hrs += '0' + hours;
        }
        else{
            hrs += hours;
        }

        let minutes = today.getMinutes();
        let min = "";
        if(minutes < 10){
            min += '0' + minutes;
        }
        else{
            min += minutes;
        }

        let seconds = today.getSeconds();
        let sec = "";
        if(seconds < 10){
            sec += '0' + seconds;
        }
        else{
            sec += seconds;
        }

        let tempTime = hrs + ':' + min + ':' + sec;
        let temp4 = tempTime.slice(0,2) + tempTime.slice(3,tempTime.length);
        let currTime = temp4.slice(0,4) + temp4.slice(5,temp4.length);

        if(userFile === ''){
            $('#status').html("fileName Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(version === ''){
            $('#status').html("Version Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(prodID === ''){
            $('#status').html("ProdID Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(sTime === ''){
            $('#status').html("Start Time Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(sDate === ''){
            $('#status').html("Start Date Missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else if(uid === ''){
            $('#status').html("UID is missing");
            document.getElementById('status').style.color = '#AA0000';
        }
        else{
            let fileName = $('#userFile').val();
            let ics = verifyICS(fileName);

            if(ics === true){
                let calJSON = '{"version":' + version + ',"prodid":"' + prodID +'","startTime":"' + startTime + '","startDate":"' + startDate + '","uid":"' + uid + '","summary":"' + summary + '","createTime":"' + currTime + '","createDate":"' + currDate + '"}';

                $.ajax({
                    url: '/addCal',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        calJSON,
                        fileName,
                    },
                    success: function (data) {   
                        console.log('HI');
                    },
                    fail: function (error) {
                        console.log(error);
                    }
                });    
                populateTable();
                $('#status').html("Calendar has been created successfully");
                document.getElementById('status').style.color = '#00BB00';
            }   
            else {
                $('#status').html("Incorrect file format (File must be .ics format)");
                document.getElementById('status').style.color = '#AA0000';
            }
        }
    });

    $('#loginBtn').on('click', function(){

        let user = $('#user').val();
        let password = $('#password').val();
        let database = $('#database').val();

        if(user === database){
            $.ajax({
                url: '/login',
                type: 'get',
                dataType: 'json',
                data: {
                    user,
                    password,
                    database,
                },
                success: function (data) {   
                    if(data.badConnect === true){    
                        $('#status').html("Invalid User");
                        document.getElementById('status').style.color = '#AA0000';
                    }
                    else{
                        storeFiles(); 
                        $('#userLogin').hide();
                        $('#query').show();
                        $('#calList').hide();
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            }); 
        }
        else{
            $('#status').html("Username must match Database name");
            document.getElementById('status').style.color = '#AA0000';
        }
    });

    $('#logoutBtn').on('click', function(){

        $.ajax({
            type: 'get',
            url: '/logout',
            success: function(data){     
                $('#userLogin').show();
                $('#query').hide();
                $('#queryList').hide();
            },
            fail: function(err){
                console.log(err)
            }
        });
    });

    $('#queryBtn').on('click', function(){
        
        $('#queryList').show();
        $('#calList').hide();
    });

    $('#clearBtn').on('click', function(){

        $.ajax({
            url: '/clearTables',
            type: 'get',
            success: function(data){

            },
            fail: function(err){
                console.log(err);
            }
        })
    });

    $('#confirmQuery').on('click', function(){

        let choice = $('#queryChoice').val();

        if(choice === "Events By Start Date"){
            $.ajax({
                type: 'get',
                url: '/eventsStartDate',
                success: function(data){
                    let eventList = data.eventList;
                    let queryStatus = "";

                    for(let i in eventList){
                        let currEvent = eventList[i];

                        let tempString = 'Event Number: ' + currEvent.event_id + '<br>Summary: ' + currEvent.summary + '<br>Start Date & Time: ' + currEvent.start_time + '<br>Location: ' + currEvent.location + '<br>Organizer: ' + currEvent.organizer + '<br>Calendar Number: ' + currEvent.cal_file;
                        queryStatus += tempString + '<br><br>';
                    }
                    $('#status').html(queryStatus);
                    document.getElementById('status').style.color = '#00BB00';
                },
                fail: function(err){
                    console.log(err);
                }
            });
        }
        else if(choice === "Start Date and Summary of an Event"){

            let calList = $('#calList').val();

            if(calList === 'Choose Calendar'){
                $('#status').html("Invalid file");
                document.getElementById('status').style.color = '#AA0000';
            }
            else{
                $.ajax({
                    type: 'get',
                    url: '/eventDateAndSum',
                    data: {
                        file: calList,
                    },
                    success: function(data){
                        let evList = data.evList;
                        let qStatus = "";

                        for(let j in evList){
                            let curEvent = evList[j];

                            let tString = 'Event Number: ' + curEvent.event_id + '<br>Summary: ' + curEvent.summary + '<br>Start Date & Time: ' + curEvent.start_time;
                            qStatus += tString + '<br><br>';
                        }
                        $('#status').html(qStatus);
                        document.getElementById('status').style.color = '#00BB00';
                    },
                    fail: function(err){
                        console.log(err);
                    }
                });
            }
        }
        else if(choice === "Conflicting start time events"){
            $.ajax({
                type: 'get',
                url: '/conflictingEvents',
                success: function(data){

                },
                fail: function(err){
                    console.log(err);
                }
            });
        }
        else if(choice === "Unique Alarm Trigger"){
            $.ajax({
                type: 'get',
                url: '/conflictingEvents',
                success: function(data){

                },
                fail: function(err){
                    console.log(err);
                }
            });
        }
        else if(choice === "Unique file prod_ID"){
            $.ajax({
                type: 'get',
                url: '/conflictingEvents',
                success: function(data){

                },
                fail: function(err){
                    console.log(err);
                }
            });
        }
        else if(choice === "Find duplicate event summaries"){
            $.ajax({
                type: 'get',
                url: '/conflictingEvents',
                success: function(data){

                },
                fail: function(err){
                    console.log(err);
                }
            });
        }
    });

    $('#queryChoice').on('change', function(){

        let choice = $('#queryChoice').val();

        if(choice === "Start Date and Summary of an Event"){
            $('#calList').show();
        }
        else{
            $('#calList').hide();
        }
    });
});