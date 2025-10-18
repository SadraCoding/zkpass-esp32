document.addEventListener('DOMContentLoaded', () => {
  // Get the two main container elements
  const activeViewCard = document.getElementById('activeViewCard'); // The Card container with PIN input
  const awaitingViewSimple = document.getElementById('awaitingViewSimple'); // The simple waiting view
  
  // Get elements inside the active view for functionality
  const pinInput = document.getElementById('pinInput');
  const verifyBtn = document.getElementById('verifyBtn');
  const minAgeValue = document.getElementById('minAgeValue');

  // Load verification request from Chrome storage
  chrome.storage.local.get('verificationRequest', (result) => {
    const request = result.verificationRequest;

    if (request) {
      // Request found: Show PIN entry view
      awaitingViewSimple.classList.add('hidden');
      activeViewCard.classList.remove('hidden');
      
      minAgeValue.textContent = request.minAge;
      pinInput.focus();

      // Set up event listeners for verification
      verifyBtn.onclick = () => verifyAge(request);
      pinInput.onkeydown = (e) => {
        if (e.key === 'Enter') verifyAge(request);
      };
    } else {
      // No request found: Show waiting state
      activeViewCard.classList.add('hidden');
      awaitingViewSimple.classList.remove('hidden');
    }
  });
});

async function verifyAge(request) {
  const pin = document.getElementById('pinInput').value.trim();
  const statusDiv = document.getElementById('status');
  const verifyBtn = document.getElementById('verifyBtn');
  const pinInput = document.getElementById('pinInput');

  // Input validation
  if (pin.length < 4 || pin.length > 8 || !/^\d+$/.test(pin)) {
    statusDiv.textContent = 'PIN must be 4–8 digits';
    statusDiv.className = 'text-red-500 font-medium';
    return;
  }

  // Pre-verification state update
  statusDiv.textContent = 'Verifying...';
  statusDiv.className = 'text-blue-500 font-medium';
  verifyBtn.disabled = true;
  pinInput.disabled = true;

  try {
    // API call to local server
    const res = await fetch('http://localhost:5000/verify-age', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ min_age: parseInt(request.minAge), pin })
    });

    const data = await res.json();
    const status = data.status || 'DENIED';

    // Send result back to the content script in the requesting tab
    chrome.tabs.sendMessage(request.tabId, {
      type: 'ZK_PASS_VERIFICATION_RESULT',
      status: status
    });

    // Update status based on API response
    if (status === 'VERIFIED') {
      statusDiv.textContent = 'Verified';
      statusDiv.className = 'text-green-500 font-medium';
    } else {
      statusDiv.textContent = 'Denied';
      statusDiv.className = 'text-red-500 font-medium';
    }

    // Clear request and close popup
    chrome.storage.local.remove('verificationRequest', () => {
      setTimeout(() => window.close(), 1200);
    });
  } catch (error) {
    // Handle connection or fetch error
    statusDiv.textContent = 'Connection error';
    statusDiv.className = 'text-red-500 font-medium';
    verifyBtn.disabled = false;
    pinInput.disabled = false;
  }
}