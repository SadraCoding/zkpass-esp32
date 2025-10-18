// Listen for verification requests from webpage
window.addEventListener('message', (event) => {
  if (event.source !== window || event.data.type !== 'ZK_PASS_VERIFY_AGE') return;
  
  // Forward to background script
  chrome.runtime.sendMessage({
    type: 'ZK_PASS_OPEN_POPUP',
    minAge: event.data.minAge
  });
});

// Listen for verification results from background/popup
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.type === 'ZK_PASS_VERIFICATION_RESULT') {
    window.postMessage({
      type: 'ZK_PASS_VERIFICATION_RESULT',
      status: request.status
    }, '*');
  }
});