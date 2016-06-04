//$('.navBar').addClass( "bounce animated")

//var arduinoIP = "149.171.143.207";

var curProfTemp;
var curProfLigh; 

var secTimer;
var safTimer;

var BPM = 70;

var timerOn = 0;
var counter = 0;
var wristBandStatus = -1;

$( function( ) {
	$( '.loadingBar' ).slideToggle( "slow" );
	// Do initial loading work here
	if( localStorage ) {
		retSetProfile( 2 );
		retSetProfile( 1 );
		if( localStorage ) {
			if( localStorage.getItem( 'genLoc' ) ) {
				$( '#Location' ).val( localStorage.getItem( 'genLoc' ) );
			}
			if( localStorage.getItem( 'password' ) ) {
				$( '#Pass' ).val( localStorage.getItem( 'password' ) );
			} else {
				localStorage.setItem( 'password', 'password' );
			}
		}
		curProfTemp = localStorage.getItem( 'proTemp1' );
		curProfLigh = localStorage.getItem( 'proLight1' );
	} else {
		alert( "Sorry, the browser you are currently using does not support the localStorage.")
	}

	var location = $( "#Location" ).val( );
	$( ".getLoc" ).text( location );
	var weatherAPI = 'http://api.openweathermap.org/data/2.5/weather';
	var data = { q: location, units: "metric", APPID: "99fc5805e5c69df8ecac05b5e85fb656"};

	function showWeather( wReport ) 
	{
		$( ".monWeather" ).text( wReport.main.temp ).append( "&#8451;" );
		$( ".monHumidity").text( wReport.main.humidity ).append( "%" );
		$( ".monCurDesc" ).text( wReport.wind.speed ).append( "m/s" );
	}

	$( ".btnSave" ).on( "click", function( ) { 
		Materialize.toast( "General settings saved!", 1000 );
		location = $( "#Location" ).val( ); 
		$( ".getLoc" ).text( location );
		data = { q: location, units: "metric", APPID: "99fc5805e5c69df8ecac05b5e85fb656"};
		var newPass = $( '#Pass' ).val( );
		localStorage.setItem( 'genLoc', location );
		localStorage.setItem( 'password', newPass );
		$.getJSON( weatherAPI, data, showWeather );
	});

	$( ".btnSaveTwo" ).on( "click", function( ) { 
		Materialize.toast( "Profile settings saved!", 1000 );
		setProfile( parseInt( $(' .selProf' ).val( ) ) ); // get current option
		console.log( parseInt( $(' .selProf' ).val( ) ) );
	});

	var sendSliders = $( '.sendSlider' );

	sendSliders.change( function( e ) {
		var him = $( this );
		var attr = him.data( "index" );
		var actionName;

		if( attr == 1 ) {
			actionName = "motor/";
			console.log( "Selected motor.");
		} else if( attr == 2 ) {
			actionName = "heater/";
			console.log( "Selected heater.");
		} else if( attr == 3 ) {
			actionName = "lighting/";
			console.log( "Selected lighting.");
		} else if( attr == 4 ) {
			actionName = "lighting2/";
			console.log( "Selected lighting 2.");
		}

		$.get( '../../arduino/' + actionName + him.val( ) );
	});

	function setProfile( profileNum ) {
		if( profileNum == 1 ) {
			localStorage.setItem( 'proNam', $( '#proName' ).val( ) );
			localStorage.setItem( 'proLight', $( '#proLight' ).val( ) );
			localStorage.setItem( 'proTemp', $( '#proTemp' ).val( ) );

			curProfTemp = localStorage.getItem( 'proTemp' );
			curProfLigh = localStorage.getItem( 'proLight' );
		} else {
			localStorage.setItem( 'proNam2', $( '#proName' ).val( ) );
			localStorage.setItem( 'proLight2', $( '#proLight' ).val( ) );
			localStorage.setItem( 'proTemp2', $( '#proTemp' ).val( ) );

			curProfTemp = localStorage.getItem( 'proTemp2' );
			curProfLigh = localStorage.getItem( 'proLight2' );
		}
		retSetProfile( profileNum );
	}

	function retSetProfile( profileNum ) {
		if( profileNum == 1 ) {
			if( localStorage ) {
				if( localStorage.getItem( 'proNam' ) ) {
					$( '#proName' ).val( localStorage.getItem( 'proNam') );
					$( '.pName1' ).text( localStorage.getItem( 'proNam' ) );
				}
				if( localStorage.getItem( 'proTemp' ) ) {
					$( '#proTemp' ).val( localStorage.getItem( 'proTemp') );
					curProfTemp = localStorage.getItem( 'proTemp' );
				}
				if( localStorage.getItem( 'proLight' ) ) {
					$( '#proLight' ).val( localStorage.getItem( 'proLight') );
					curProfLigh = localStorage.getItem( 'proLight' );
				}
			}	
		} 

		else {
			if( localStorage ) {
				if( localStorage.getItem( 'proNam2' ) ) {
					$( '#proName' ).val( localStorage.getItem( 'proNam2') );
					$( '.pName2' ).text( localStorage.getItem( 'proNam2' ) );
				}
				if( localStorage.getItem( 'proTemp2' ) ) {
					$( '#proTemp' ).val( localStorage.getItem( 'proTemp2') );
					curProfTemp = localStorage.getItem( 'proTemp2' );
				}
				if( localStorage.getItem( 'proLight2' ) ) {
					$( '#proLight' ).val( localStorage.getItem( 'proLight2') );
					curProfLigh = localStorage.getItem( 'proLight2' );
				}
			} else {		// Profile 2 is empty
				$( '#proName' ).val( "Name" );
				$( '.pName2' ).val( "Profile 2" );
			}
		}
	}

	function retrieveSensorData2( ) {
		var lightVal, tempVal;
		$.get( '../../arduino/analog/0/', function( e ) {
			tempVal = parseInt( e );
			lightVal = ( e / 255 ) * 100;
			$( ".retLight").text( lightVal + '%' );
		} );
		$.get( '../../arduino/analog/3/', function( e ) {
			tempVal = parseInt( e );
			lightVal = ( e / 255 ) * 100;
			$( ".retLight2").text( lightVal + '%' );
		} );
		$.get( '../../arduino/temp/-1', function( e ) {
			tempVal = parseInt( e );
			$( ".retTemp" ).text( e ).append( "&#8451;" );
		} );
	}

	function retrieveSensorData( ) 
	{
		$.get( '../../arduino/profile/-1', function( e ) {
			tempVal = parseInt( e );
			if( tempVal == 0 ) {							// profile 1
				$( '.profile_2' ).hide( );
				$( '.profile_1' ).show( ); 
			} else {										// profile 2 
				$( '.profile_1' ).hide( );
				$( '.profile_2' ).show( );
			}
			console.log( "Current Profile (Wristband): " + tempVal );
		} );
		$.get( '../../arduino/heartbeat/-1', function( e ) {
			tempVal = parseInt( e );
			tempVal = tempVal;
			if( tempVal != -1 ) {
				$( ".monCurBPM" ).text( tempVal ).append( " bpm" );
				BPM = tempVal;
				window.clearTimeout( safTimer );
				timerOn = 0;
				wristBandStatus = -1;
			} else {
				if( timerOn == 0 ) {
					safTimer = window.setTimeout( triggerSafAlert, 10000 );
					wristBandStatus = 0;
					timerOn = 1;
					if( document.getElementById( 'customSet' ) ) {

					} else { 
						turnOff( );
					}
				}	
				console.log( "I do not detect a wristband." );
				//begin timeout timer to trigger security alert
			}
		} );
/*		$.get( '../../arduino/motion/-1', function( e ) {
			tempVal = parseInt( e );
			if( ( tempVal == 1 && wristBandStatus == 1 ) && ( document.getElementById( 'btnAutoS' ).checked == 1 || 
					document.getElementById( 'forceSec' ).checked == 1 ) ) {
				triggerSecAlert( );
				console.log( "SECURITY ALERT" );
			}
		} );*/
	}

	$( '#btnAuto' ).on( 'change', function( ) {
		if( document.getElementById( 'btnAuto' ).checked == 0 ) {  
			turnOff( );
			console.log( "Automatic preferences turned off." );
		}
	});

	function turnOff( ) {
		$.get( '../../arduino/lighting/0' );
		$.get( '../../arduino/lighting2/0' );
		$.get( '../../arduino/motor/0' );
		$.get( '../../arduino/heater/0' );
		console.log( "meant to turn off everything." );
	}

	function triggerSafAlert( ) {		// Safety / Health
		// send email or text message.
		//$( '#modal1' ).closeModal( );
		//$( '#modal1' ).openModal( );
		wristBandStatus = 1;
		timerOn = 0;
		turnOff( );
		console.log( "Wrist band is out of range.");
	}

	function triggerSecAlert( ) {		// Security
		// send email or text message.
		//$( '#modal2' ).openModal( );
		$( '#modal2' ).openModal( );
		console.log( "Security Alert Triggered!" );
		$.get( '../../arduino/alert/2' ); 
		wristBandStatus = 0;
	}

	function triggerSafetyAlert( ) {
		$.get( '../../arduino/alert2/2' );
		console.log( "Health Alert Triggered" );
	}

	$( '.healthAlert' ).on( 'click', function( ) { 
		triggerSafetyAlert( );
	} );

	$.getJSON( weatherAPI, data, showWeather );
	$('.modal-trigger').leanModal();
	$( '#modal3' ).openModal({dismissible: false});
	setInterval( retrieveSensorData, 5000 );
	setInterval( retrieveSensorData2, 10000 );


	var pass = $( '#passwd' );
	pass.keyup( function( ) { 
		if( pass.val( ) == localStorage.getItem( 'password' ) ) {
			$( '#modal3' ).closeModal( );
		}
	});

	var newDate = new Date( );
	var datetime = newDate.toLocaleTimeString( 'en-US', 
		{ hour12: false, hour:'numeric', minute: 'numeric'} );
	$( '.curTime' ).html( "Time: " + datetime );

    $( 'select' ).material_select( );

    $( '.selProf' ).change( function( ) {
		var optionSelected = $(this).find("option:selected");
		var valueSelected  = optionSelected.val();
		var textSelected   = optionSelected.text();
		retSetProfile( valueSelected );
    });

    $( '#customSet' ).on( 'change', function( ) {
    	$( '.hidden' ).toggle( 500 );
    	$( '#btnAuto' ).attr( 'checked', false );
    	turnOff( );
    } );
	
});

