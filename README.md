A very lightweight gc listener for Node.JS, providing a way to listen 'before' and 'after' events on v8 GC.

####Sample code

```javascript
var gc = require( 'gc-listener' );

var gcTimes = 0;

gc.before( function() {
  console.log( 'before gc #' + gcTimes );
  showMem();
} );

gc.after( function() {
  ++gcTimes;
  console.log( 'after gc #' + gcTimes );
  showMem();
} );

var ary = [];

var interval = setInterval( function() {
  if( gcTimes < 4 ) {
    for( var i=0; i<1000; ++i ) {
      ary.push( { value : Math.random() } );
    }
  }
  else {
    console.log( 'stop pushing new values!' );
    clearInterval( interval );
  }
}, 1 );

var OneMega = 1024 * 1024;

function showMem() {
  var usage = process.memoryUsage();
  console.log( 'rss       : ', ( usage.rss        / OneMega ).toFixed(2), ' MB' );
  console.log( 'heapTotal : ', ( usage.heapTotal  / OneMega ).toFixed(2), ' MB' );
  console.log( 'heapUsed  : ', ( usage.heapUsed   / OneMega ).toFixed(2), ' MB' );
}
```
