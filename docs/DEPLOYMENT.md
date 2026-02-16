# Deployment Guide for OpenModScan GitHub Pages

This guide explains how to deploy the OpenModScan website to GitHub Pages.

## Prerequisites

- GitHub account with access to the OpenModScan repository
- The `docs` folder with all website files

## Deployment Steps

### 1. Enable GitHub Pages

1. Navigate to your repository on GitHub: https://github.com/sanny32/OpenModScan
2. Click on **Settings** tab
3. In the left sidebar, click **Pages**
4. Under **Source**, select:
   - **Branch**: `main` (or your default branch)
   - **Folder**: `/docs`
5. Click **Save**

### 2. Wait for Deployment

- GitHub will automatically build and deploy your site
- This process typically takes 1-2 minutes
- You'll see a notification when the site is ready

### 3. Access Your Site

Once deployed, your site will be available at:
```
https://sanny32.github.io/OpenModScan/
```

## Custom Domain (Optional)

If you want to use a custom domain:

1. Create a file named `CNAME` in the `docs` folder
2. Add your domain name (e.g., `openmodscan.com`)
3. Configure DNS settings with your domain provider:
   - Add a CNAME record pointing to `sanny32.github.io`
4. Wait for DNS propagation (up to 24-48 hours)

## Updating the Site

To update the website:

1. Make changes to files in the `docs` folder
2. Commit and push changes to GitHub:
   ```bash
   git add docs/
   git commit -m "Update website"
   git push origin main
   ```
3. GitHub Pages will automatically rebuild and deploy the updated site

## Troubleshooting

### Site Not Loading

- Check that GitHub Pages is enabled in repository settings
- Verify the correct branch and folder are selected
- Wait a few minutes for deployment to complete
- Check the Actions tab for build errors

### Images Not Displaying

- Ensure all image paths are relative (not absolute)
- Verify images are committed to the repository
- Check file names match exactly (case-sensitive)

### Styles Not Applied

- Clear browser cache
- Check that `style.css` is in the same directory as `index.html`
- Verify the stylesheet link in HTML is correct

## File Structure

```
docs/
├── index.html              # Main page
├── 404.html               # Error page
├── style.css              # Stylesheet
├── manifest.json          # PWA manifest
├── robots.txt             # SEO: robots
├── sitemap.xml            # SEO: sitemap
├── .nojekyll              # Disable Jekyll processing
├── logo.png               # Logo (128x128)
├── favicon-32x32.png      # Favicon
├── favicon-16x16.png      # Small favicon
├── screenshot-main.png    # Main screenshot
├── screenshot-log.png     # Logging screenshot
├── screenshot-scanner.png # Scanner screenshot
├── README.md             # Documentation
└── DEPLOYMENT.md         # This file
```

## Performance Optimization

The site is already optimized with:
- Minified CSS
- Compressed images
- Lazy loading for images
- Responsive design
- SEO meta tags
- PWA manifest

## Analytics (Optional)

To add Google Analytics:

1. Create a Google Analytics account
2. Get your tracking ID (GA4)
3. Add the tracking code to `index.html` before `</head>`:

```html
<!-- Google Analytics -->
<script async src="https://www.googletagmanager.com/gtag/js?id=G-XXXXXXXXXX"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());
  gtag('config', 'G-XXXXXXXXXX');
</script>
```

## Support

For issues or questions:
- Create an issue on GitHub: https://github.com/sanny32/OpenModScan/issues
- Contact: mail@ananev.org

## License

The website is part of OpenModScan and is licensed under MIT License.
