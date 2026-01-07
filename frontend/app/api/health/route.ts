import { NextResponse } from 'next/server';

const BACKEND_URL = 'http://57.159.30.64/api';

export async function GET() {
    try {
        const response = await fetch(`${BACKEND_URL}/health`);
        const data = await response.json();
        return NextResponse.json(data);
    } catch (error) {
        return NextResponse.json({ status: 'error' }, { status: 500 });
    }
}
