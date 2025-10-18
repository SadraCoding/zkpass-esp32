// Handle popup opening requests from content script
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.type === 'ZK_PASS_OPEN_POPUP') {
    // Store verification request data
    chrome.storage.local.set({
      verificationRequest: {
        minAge: request.minAge,
        tabId: sender.tab.id
      }
    }, () => {
      // Open popup immediately
      chrome.action.openPopup();
    });
    return true;
  }
});

// Clean up storage when popup is closed
chrome.windows.onRemoved.addListener(() => {
  chrome.storage.local.remove('verificationRequest');
});