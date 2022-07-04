import blink from 'blink-detection';
await blink.loadModel();

const videoElement = document.querySelector('video');

const init = async () => {
  // Using the default webcam
  await gaze.setUpCamera(videoElement);

  // Or, using more camera input devices
//   const mediaDevices = await navigator.mediaDevices.enumerateDevices();
//   const camera = mediaDevices.find(
//     (device) =>
//       device.kind === 'videoinput' &&
//       device.label.includes(/* The label from the list of available devices*/)
//   );

//   await gaze.setUpCamera(videoElement, camera.deviceId);
};

const predict = async () => {
    const blinkPrediction = await blink.getBlinkPrediction();
    console.log('Blink: ', blinkPrediction); // will return an object indicating the booleans for different states
    // expect blinkPrediction to be {
    //  blink: boolean,
    //  wink: boolean,
    //  longBlink: boolean,
    //  left: boolean,
    //  right: boolean,
    //  rate: number
    // }
    if (blinkPrediction.blink) {
        // do something when the user blinks
    }
    let raf = requestAnimationFrame(predict);
};

module.exports = {

}