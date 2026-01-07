# ByteCode Compiler - Frontend

Professional web interface for the ByteCode Compiler featuring a terminal-style code editor, comprehensive documentation, and Azure backend integration.

## Features

- ✨ **Beautiful Landing Page** - Calm, attractive design with smooth animations
- 💻 **Terminal Interface** - Monaco Editor with terminal aesthetics
- 📚 **Documentation Viewer** - Interactive docs with syntax highlighting  
- 🎨 **Full Customization** - Dark/Light themes, terminal colors, fonts, sizes
- ⚡ **Optimized Performance** - Next.js with server-side rendering
- 🔗 **Azure Integration** - Real-time compilation via REST API

## Tech Stack

- **Framework**: Next.js 14 (App Router)
- **Language**: TypeScript
- **Styling**: Tailwind CSS
- **Editor**: Monaco Editor (VS Code editor)
- **Animation**: Framer Motion
- **Icons**: Lucide React

## Quick Start

```bash
# Install dependencies
cd frontend
npm install

# Run development server
npm run dev

# Open http://localhost:3000
```

## Project Structure

```
frontend/
├── app/
│   ├── page.tsx              # Landing page
│   ├── layout.tsx            # Root layout
│   ├── compiler/
│   │   └── page.tsx          # Terminal interface
│   └── docs/
│       └── page.tsx          # Documentation viewer
├── components/
│   ├── Header.tsx            # Navigation header
│   ├── Footer.tsx            # Credits footer
│   ├── ThemeToggle.tsx       # Dark/Light mode
│   ├── Terminal.tsx          # Code editor
│   └── DocsNav.tsx           # Docs sidebar
├── lib/
│   ├── api.ts                # Azure API client
│   └── themes.ts             # Terminal themes
└── public/
    └── (assets)
```

## Environment Variables

Create `.env.local`:

```env
NEXT_PUBLIC_API_URL=http://57.159.30.64/api
NEXT_PUBLIC_GITHUB_URL=https://github.com/IsVohi/ByteCode-Compiler
NEXT_PUBLIC_PORTFOLIO_URL=YOUR_PORTFOLIO_URL
```

## Pages

### Landing Page (`/`)
- Hero section with compiler description
- Two CTA buttons: Open Compiler | Read Docs
- Features showcase
- Footer with credits

### Compiler (`/compiler`)
- Monaco Editor with terminal theme
- Help panel (syntax reference)
- File upload (.src files)
- Command palette
- Output display
- Customization sidebar

### Documentation (`/docs`)
- Navigation sidebar
- Markdown renderer
- Search functionality
- Syntax highlighting

## Header Links

- GitHub Repository
- Portfolio (customizable)
- Theme Toggle dark/Light)

## Footer

Created by **Vikas Sharma** © 2024  
All rights reserved.

## Deployment

### Vercel (Recommended)

```bash
# Install Vercel CLI
npm i -g vercel

# Deploy
vercel

# Production
vercel --prod
```

Configuration is in `vercel.json`.

### Environment Variables on Vercel

Add these in Vercel dashboard:
- `NEXT_PUBLIC_API_URL`
- `NEXT_PUBLIC_GITHUB_URL`
- `NEXT_PUBLIC_PORTFOLIO_URL`

## Development

```bash
# Install dependencies
npm install

# Run dev server
npm run dev

# Build for production
npm run build

# Start production server
npm start

# Lint code
npm run lint
```

##Terminal Customization

Users can customize:
- Background colors
- Text colors
- Font family
- Font size
- Cursor style
- Line height

Themes are saved to localStorage.

## API Integration

The frontend connects to your Azure VM API:

```typescript
const response = await fetch(`${API_URL}/compile`, {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ code })
});
```

## Performance

- Server-side rendering (SSR)
- Code splitting
- Image optimization
- Font optimization
- Lazy loading

## Browser Support

- Chrome (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)

## License

MIT

---

**Note**: This is a professional showcase project demonstrating full-stack development capabilities.
