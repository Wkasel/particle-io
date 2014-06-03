var five = require("johnny-five"),
  Spark = require("../lib/spark"),
  temporal = require('temporal'),
  board;

// Create Johnny-Five board connected via Spark
board = new five.Board({
  io: new Spark({
    token: process.env.SPARK_TOKEN,
    deviceId: process.env.SPARK_DEVICE_ID
  })
});

// The board's pins will not be accessible until
// the board has reported that it is ready
board.on("ready", function() {
  console.log("CONNECTED");

  // Create a new `servo` hardware instance.
  var servo  = new five.Servo({
      pin  : "D0"
  }).stop();

  temporal.queue([
    {
      delay: 0,
      task: function(){
        // min()
        //
        // set the servo to the minimum degrees
        // defaults to 0
        //
        servo.min();
      }
    },{
      delay: 1000,
      task: function(){
        // max()
        //
        // set the servo to the maximum degrees
        // defaults to 180
        //
        servo.max();
      }
    },{
      delay: 1000,
      task: function(){
        // center()
        //
        // centers the servo to 90°
        //
        servo.center();
      }
    },{
      delay: 1000,
      task: function(){
        // sweep( obj )
        //
        // Perform a min-max cycling servo sweep (defaults to 0-180)
        // optionally accepts an object of sweep settings:
        // {
        //    lapse: time in milliseconds to wait between moves
        //           defaults to 500ms
        //    degrees: distance in degrees to move
        //           defaults to 10°
        // }
        //
        servo.sweep();
      }
    },{
      delay: 5000,
      task: function(){
        // stop(  )
        //
        // Stop a moving servo
        servo.stop();
      }
    },{
      delay: 1000,
      task: function(){
        // to( deg )
        //
        // Moves the servo to position by degrees
        //
        servo.to( 15 );
      }
    },{
      delay: 1000,
      task: function(){
        // step ( deg )
        //
        // Move servo relative to current position by degrees

        temporal.loop(500,function(){
          if(this.called > 10){
            process.exit(0);
            this.stop();
          }

          servo.step( 15 );
        });
        
      }
    }
  ]);

});