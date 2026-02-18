// Tab switching
function initTabs() {
    const tabButtons = document.querySelectorAll('.tab-btn');
    const tabPanels = document.querySelectorAll('.tab-panel');

    tabButtons.forEach(function(btn) {
        btn.addEventListener('click', function() {
            var tabId = this.getAttribute('data-tab');

            tabButtons.forEach(function(b) { b.classList.remove('active'); });
            tabPanels.forEach(function(p) { p.classList.remove('active'); });

            this.classList.add('active');
            document.getElementById('tab-' + tabId).classList.add('active');
        });
    });
}

// Fetch latest release information from GitHub
async function fetchLatestRelease() {
    const GITHUB_API = 'https://api.github.com/repos/sanny32/OpenModScan/releases/latest';

    try {
        console.log('Fetching latest release from GitHub...');
        const response = await fetch(GITHUB_API);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const release = await response.json();
        updateDownloadLinks(release);
    } catch (error) {
        console.error('Error fetching release:', error);
        console.warn('Download links will point to releases page instead of direct downloads');
    }
}

function updateDownloadLinks(release) {
    const assets = release.assets;

    // Find specific file patterns
    const files = {
        winx64Qt6: assets.find(a => a.name.match(/qt6-omodscan-.*_x64\.exe$/i)),
        winx64Qt5: assets.find(a => a.name.match(/qt5-omodscan-.*_x64\.exe$/i)),
        winx86: assets.find(a => a.name.match(/qt5-omodscan-.*_x86\.exe$/i)),
        debQt6: assets.find(a => a.name.match(/qt6-omodscan_.*_amd64\.deb$/i)),
        debQt5: assets.find(a => a.name.match(/qt5-omodscan_.*_amd64\.deb$/i)),
        rpmQt6: assets.find(a => a.name.match(/qt6-omodscan-.*\.x86_64\.rpm$/i)),
        rpmPubkey: assets.find(a => a.name.match(/qt6-omodscan\.rpm\.pubkey$/i)),
        flatpak: assets.find(a => a.name.match(/io\.github\.sanny32\.omodscan.*\.flatpak$/i))
    };

    // Helper function to update link
    function updateLink(id, file, name) {
        const link = document.getElementById(id);
        if (link && file) {
            link.href = file.browser_download_url;
        } else if (link && !file) {
            console.warn(`✗ File not found for ${name}`);
        }
    }

    // Update all download links
    updateLink('win-x64-qt6-link', files.winx64Qt6, 'Windows x64 Qt6');
    updateLink('win-x64-qt5-link', files.winx64Qt5, 'Windows x64 Qt5');
    updateLink('win-x86-qt5-link', files.winx86, 'Windows x86 Qt5');
    updateLink('deb-qt6-link', files.debQt6, 'DEB Qt6');
    updateLink('deb-qt5-link', files.debQt5, 'DEB Qt5');
    updateLink('rpm-qt6-link', files.rpmQt6, 'RPM Qt6');
    updateLink('rpm-pubkey-link', files.rpmPubkey, 'RPM Public Key');
    updateLink('flatpak-link', files.flatpak, 'Flatpak');

    // Update all install commands with actual file names
    updateInstallCommands(files);

    // Update version display if available
    if (release.tag_name) {
        updateVersionDisplay(release.tag_name);
    }
}

function updateVersionDisplay(version) {
    // Update the download subtitle to include version
    const subtitle = document.querySelector('.download-subtitle');
    if (subtitle) {
        subtitle.textContent = `Get the latest version (${version}) for your platform`;
    }
}

function updateInstallCommands(files) {
    if (files.debQt5) { 
        const debName = files.debQt5.name;
        document.querySelectorAll('.qt5-deb-package-name').forEach(el => {
            el.textContent = debName;
        });
    }

    if (files.debQt6) { 
        const debName = files.debQt6.name;
        document.querySelectorAll('.qt6-deb-package-name').forEach(el => {
            el.textContent = debName;
        });
    }

    if (files.rpmQt6) { 
        const rpmName = files.rpmQt6.name;
        document.querySelectorAll('.qt6-rpm-package-name').forEach(el => {
            el.textContent = rpmName;
        });
    }
}

function initCopyButtons() {
  document.querySelectorAll('.install-instructions .code-block .copy-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        const codeEl = btn.closest('.code-block').querySelector('code');
        if (!codeEl) return;

        navigator.clipboard.writeText(codeEl.innerText.trim()).then(() => {
            btn.classList.add('copied');
            btn.innerHTML = `<svg viewBox="0 0 16 16" fill="currentColor"><path d="M13.78 4.22a.75.75 0 0 1 0 1.06l-7.25 7.25a.75.75 0 0 1-1.06 0L2.22 9.28a.751.751 0 0 1 .018-1.042.751.751 0 0 1 1.042-.018L6 10.94l6.72-6.72a.75.75 0 0 1 1.06 0Z"/></svg>`;

            setTimeout(() => {
                btn.classList.remove('copied');
                btn.innerHTML = `
                    <svg viewBox="0 0 16 16" fill="currentColor">
                        <path d="M0 6.75C0 5.784.784 5 1.75 5h1.5a.75.75 0 0 1 0 1.5h-1.5a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-1.5a.75.75 0 0 1 1.5 0v1.5A1.75 1.75 0 0 1 9.25 16h-7.5A1.75 1.75 0 0 1 0 14.25Z"/>
                        <path d="M5 1.75C5 .784 5.784 0 6.75 0h7.5C15.216 0 16 .784 16 1.75v7.5A1.75 1.75 0 0 1 14.25 11h-7.5A1.75 1.75 0 0 1 5 9.25Zm1.75-.25a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-7.5a.25.25 0 0 0-.25-.25Z"/>
                    </svg>
                `;
            }, 1500);
        });
    });
});
}


// Initialize when page loads
document.addEventListener('DOMContentLoaded', function() {
    initTabs();
    fetchLatestRelease();
    initCopyButtons();
});
